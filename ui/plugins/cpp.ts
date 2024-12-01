import type { Plugin } from 'vite';
import { resolve, dirname } from 'path';
import { promisify } from 'util';
import fs from 'fs/promises';
import { pascalCase } from "change-case";
import mime from 'mime';
import type { OutputBundle, OutputAsset, OutputChunk } from 'rollup';
import { gzip } from '@gfx/zopfli';

const zopfliCompress = promisify(gzip);

interface CompressedOutput {
    name: string;
    length: number;
    array: string;
    contentType: string;
}

interface Asset {
    path: string;
    name: string;
    content: string | Buffer | Uint8Array;
    contentType: string;
    type: string;
}

interface CppPluginOptions {
    outPrefix?: string;   // Prefix for output files
    basePath?: string;    // Base URL path (e.g., '/ui', '/app', etc.)
    staticDir?: string;   // Directory for static assets
    buildDir?: string;    // Directory for build output
    outputDir?: string;   // Directory for C++ headers
}

function hexdump(buffer: Uint8Array): string {
    const lines: string[] = [];
    for (let i = 0; i < buffer.length; i += 16) {
        const block = buffer.slice(i, i + 16);
        const hexArray = Array.from(block).map(value =>
            "0x" + value.toString(16).padStart(2, "0")
        );
        lines.push(`  ${hexArray.join(", ")}`);
    }
    return lines.join(",\n");
}

async function compressAsset(input: string | Buffer | Uint8Array, fileName: string, contentType?: string): Promise<CompressedOutput> {
    const options = {
        blocksplitting: true,
        blocksplittinglast: false,
        blocksplittingmax: 15,
        verbose: false
    };

    const result = await zopfliCompress(Buffer.from(input), options);
    console.info(`${fileName} compressed ${result.length} bytes`);

    return {
        name: fileName.replace(/[.-]/g, "_").toUpperCase(),
        length: result.length,
        array: hexdump(result),
        contentType: contentType || mime.getType(fileName) || 'application/octet-stream'
    };
}

export function cppPlugin(options: CppPluginOptions = {}): Plugin {
    const assets = new Map<string, Asset>();

    const basePath = options.basePath || '';

    // Resolve all paths relative to the current file's directory
    const staticDir = resolve(__dirname, options.staticDir || '../static');
    const buildDir = resolve(__dirname, options.buildDir || '../build');
    const outputDir = resolve(__dirname, options.outputDir || '../../src');
    const outPrefix = options.outPrefix || 'web_';

    function getGroupName(path: string, type: string): string {
        const dir = dirname(path);
        if (dir === '.') return `${outPrefix}${type}`;
        return outPrefix + dir.replace(/\//g, '_') + '_' + type;
    }

    return {
        name: 'cpp',
        enforce: 'post',

        // Collect static files
        async buildStart() {
            try {
                const files = await fs.readdir(staticDir);
                for (const file of files) {
                    if (file.startsWith('.')) continue;

                    const filePath = resolve(staticDir, file);
                    const stats = await fs.stat(filePath);
                    if (stats.isFile()) {
                        const content = await fs.readFile(filePath);
                        const ext = file.split('.').pop()?.toLowerCase() || '';
                        assets.set(file, {
                            path: file,
                            name: file.replace(/[.-]/g, '_'),
                            content,
                            contentType: mime.getType(file) || 'application/octet-stream',
                            type: ext
                        });
                    }
                }
                console.log(`Captured ${assets.size} static assets from ${staticDir}`);
            } catch (error) {
                if (error.code !== 'ENOENT') {
                    console.error(`Error reading static directory ${staticDir}:`, error);
                }
            }
        },

        async writeBundle(options: any, bundle: OutputBundle) {
            // Only process client builds
            const isClientBuild = Object.keys(bundle).some(key =>
                key.includes('immutable/entry/app') ||
                key.includes('immutable/entry/start')
            );
            if (!isClientBuild) return;

            // Group assets by directory path and type
            const groupedAssets = new Map<string, Asset[]>();

            // Add bundle files to assets
            for (const [fileName, file] of Object.entries(bundle)) {
                if (fileName.split('/')[0].startsWith('.')) continue;
                if (fileName.endsWith('.json')) continue;

                const content = file.type === 'chunk'
                    ? (file as OutputChunk).code
                    : (file as OutputAsset).source;
                const ext = fileName.split('.').pop()?.toLowerCase() || '';

                const asset = {
                    path: fileName,
                    name: fileName.replace(/[\/\\]/g, '_').replace(/[.-]/g, '_'),
                    content,
                    contentType: file.type === 'chunk'
                        ? 'application/javascript'
                        : mime.getType(fileName) || 'application/octet-stream',
                    type: ext
                };

                const groupName = getGroupName(fileName, ext);
                const group = groupedAssets.get(groupName) || [];
                group.push(asset);
                groupedAssets.set(groupName, group);
            }

            // Add HTML files from build
            try {
                const buildFiles = await fs.readdir(buildDir);
                for (const file of buildFiles) {
                    if (!file.endsWith('.html')) continue;

                    const content = await fs.readFile(resolve(buildDir, file), 'utf-8');
                    const asset = {
                        path: file,
                        name: file.replace('.html', '_html').replace(/[.-]/g, '_'),
                        content,
                        contentType: 'text/html',
                        type: 'html'
                    };

                    const groupName = getGroupName(file, 'html');
                    const group = groupedAssets.get(groupName) || [];
                    group.push(asset);
                    groupedAssets.set(groupName, group);
                }
            } catch (error) {
                console.error(`Error reading build directory ${buildDir}:`, error);
                return;
            }

            // Add static assets
            for (const asset of assets.values()) {
                const groupName = getGroupName(asset.path, asset.type);
                const group = groupedAssets.get(groupName) || [];
                group.push(asset);
                groupedAssets.set(groupName, group);
            }

            // Generate headers
            try {
                await fs.mkdir(outputDir, { recursive: true });

                const routes: string[] = [];
                const htmlRoutes: string[] = [];

                // Generate a header file for each group
                for (const [groupName, groupAssets] of groupedAssets) {
                    let header = `/*
 * Binary arrays for the Web UI ${groupName} files.
 * Uses Zopfli compression for optimal size and improved speed.
 */

#pragma once
#include <ESPAsyncWebServer.h>
#include <Arduino.h>

`;

                    for (const asset of groupAssets) {
                        const compressed = await compressAsset(asset.content, asset.name, asset.contentType);

                        header += `// ${asset.path}\n`;
                        header += `const uint16_t ${compressed.name}_L = ${compressed.length};\n`;
                        header += `const uint8_t ${compressed.name}[] PROGMEM = {\n${compressed.array}\n};\n\n`;
                        header += `inline void serve${pascalCase(asset.name)}(AsyncWebServerRequest* request) {\n`;
                        header += `  AsyncWebServerResponse *response = request->beginResponse_P(200, "${asset.contentType}", ${compressed.name}, ${compressed.name}_L);\n`;
                        header += `  response->addHeader(F("Content-Encoding"), "gzip");\n`;
                        header += `  request->send(response);\n`;
                        header += `}\n\n`;

                        // Add routes
                        if (asset.type === 'html') {
                            const routePath = asset.path === 'index.html'
                                ? basePath + '/'
                                : basePath + '/' + asset.path.slice(0, -5);  // Remove .html
                            htmlRoutes.push(`    server->on("${routePath}", HTTP_GET, serve${pascalCase(asset.name)});`);
                            if (routePath !== basePath + '/') {
                                htmlRoutes.push(`    server->on("${routePath}.html", HTTP_GET, serve${pascalCase(asset.name)});`);
                            }
                        } else {
                            routes.push(`    server->on("${basePath}/${asset.path}", HTTP_GET, serve${pascalCase(asset.name)});`);
                        }
                    }

                    await fs.writeFile(resolve(outputDir, `${groupName}.h`), header);
                }

                // Generate routes header
                const routesHeader = `#pragma once

#include <ESPAsyncWebServer.h>
${Array.from(groupedAssets.keys()).map(group => `#include "${group}.h"`).join('\n')}

inline void setupRoutes(AsyncWebServer* server) {
${routes.join('\n')}

    // HTML routes
${htmlRoutes.join('\n')}
}`;

                await fs.writeFile(resolve(outputDir, `${outPrefix}routes.h`), routesHeader);
                console.log(`Generated C++ headers in ${outputDir}`);
            } catch (error) {
                console.error(`Error writing output files to ${outputDir}:`, error);
            }
        }
    };
}
