import type { Plugin } from 'vite';
import { resolve } from 'path';
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

function hexdump(buffer: Uint8Array): string {
    let lines: string[] = [];
    for (let i = 0; i < buffer.length; i += 16) {
        let block = buffer.slice(i, i + 16);
        let hexArray: string[] = [];
        for (let value of block) {
            hexArray.push("0x" + value.toString(16).padStart(2, "0"));
        }
        let line = `  ${hexArray.join(", ")}`;
        lines.push(line);
    }
    return lines.join(",\n");
}

async function cppCompressed(input: string | Buffer | Uint8Array, fileName: string, contentType?: string): Promise<CompressedOutput> {
    const options = {
        blocksplitting: true,
        blocksplittinglast: false,
        blocksplittingmax: 15,
        verbose: false
    };

    const result = await zopfliCompress(Buffer.from(input), options);
    console.info(fileName + " compressed " + result.length + " bytes");
    const array = hexdump(result);
    const headerName = fileName.replace(/[.-]/g, "_").toUpperCase();

    return {
        name: headerName,
        length: result.length,
        array: array,
        contentType: contentType || mime.getType(fileName) || 'application/octet-stream'
    };
}

async function isDirectory(path: string): Promise<boolean> {
    try {
        const stat = await fs.stat(path);
        return stat.isDirectory();
    } catch (error) {
        return false;
    }
}

export function cppPlugin(): Plugin {
    return {
        name: 'cpp',
        async writeBundle(options: any, bundle: OutputBundle) {
            const isClientBuild = Object.keys(bundle).some(key =>
                key.includes('immutable/entry/app') ||
                key.includes('immutable/entry/start')
            );

            if (!isClientBuild) return;

            const outputDir = resolve(__dirname, '../../src');
            await fs.mkdir(outputDir, { recursive: true });

            // Group files by directory
            const filesByDir = new Map<string, {
                route: string;
                name: string;
                content: string | Buffer | Uint8Array;
                contentType: string;
                type: string;
            }[]>();

            // Process bundle files
            for (const [fileName, file] of Object.entries(bundle)) {
                // Skip hidden directories (starting with .)
                if (fileName.split('/')[0].startsWith('.')) continue;

                const ext = fileName.split('.').pop()?.toLowerCase() || '';
                if (ext === 'json') continue;  // Skip JSON files early

                const dir = fileName.includes('/') ? fileName.split('/')[0] : 'root';
                const files = filesByDir.get(dir) || [];

                if (file.type === 'chunk') {
                    files.push({
                        route: `/ui/${fileName}`,
                        name: fileName.replace(/[\/\\]/g, '_').replace(/[.-]/g, '_'),
                        content: (file as OutputChunk).code,
                        contentType: 'application/javascript',
                        type: 'js'
                    });
                } else if (file.type === 'asset') {
                    const asset = file as OutputAsset;
                    files.push({
                        route: `/ui/${fileName}`,
                        name: fileName.replace(/[\/\\]/g, '_').replace(/[.-]/g, '_'),
                        content: asset.source,
                        contentType: mime.getType(fileName) || 'application/octet-stream',
                        type: ext
                    });
                }

                if (files.length > 0) {
                    filesByDir.set(dir, files);
                }
            }

            // Process build directory files
            try {
                const buildDir = resolve(__dirname, '../build');
                const buildFiles = await fs.readdir(buildDir);
                const files = filesByDir.get('root') || [];

                // Process all files in build directory
                for (const file of buildFiles) {
                    const filePath = resolve(buildDir, file);
                    if (await isDirectory(filePath)) continue;

                    const content = await fs.readFile(filePath);
                    const ext = file.split('.').pop()?.toLowerCase() || '';
                    const contentType = mime.getType(file) || 'application/octet-stream';
                    const isText = contentType.startsWith('text/') || contentType === 'application/javascript';

                    files.push({
                        route: `/ui/${file}`,
                        name: file.replace(/[.-]/g, '_'),
                        content: isText ? content.toString('utf-8') : content,
                        contentType,
                        type: ext
                    });
                }

                if (files.length > 0) {
                    filesByDir.set('root', files);
                }
            } catch (error) {
                console.error('Error reading build files:', error);
            }

            // Generate header files
            const routes: {route: string, func: string}[] = [];
            const htmlRoutes: {route: string, func: string}[] = [];

            for (const [dir, files] of filesByDir.entries()) {
                const filesByType = new Map<string, typeof files>();
                for (const file of files) {
                    const typeFiles = filesByType.get(file.type) || [];
                    typeFiles.push(file);
                    filesByType.set(file.type, typeFiles);
                }

                for (const [type, typeFiles] of filesByType.entries()) {
                    const headerName = `ui_${dir}_${type}.h`;
                    let headerContent = `/*
 * Binary arrays for the Web UI ${dir} directory (${type.toUpperCase()} files).
 * Zopfli compression is used for smaller size and improved speeds.
 */

#pragma once
#include <ESPAsyncWebServer.h>
#include <Arduino.h>

`;

                    const processedNames = new Set();

                    for (const file of typeFiles) {
                        // Skip duplicate content (from extensionless routes)
                        if (processedNames.has(file.name)) continue;
                        processedNames.add(file.name);

                        const compressed = await cppCompressed(file.content, file.name, file.contentType);

                        headerContent += `// ${file.name}\n`;
                        headerContent += `const uint16_t ${compressed.name}_L = ${compressed.length};\n`;
                        headerContent += `const uint8_t ${compressed.name}[] PROGMEM = {\n${compressed.array}\n};\n\n`;
                        headerContent += `inline void serve${pascalCase(file.name)}(AsyncWebServerRequest* request) {\n`;
                        headerContent += `  AsyncWebServerResponse *response = request->beginResponse_P(200, "${file.contentType}", ${compressed.name}, ${compressed.name}_L);\n`;
                        headerContent += `  response->addHeader(F("Content-Encoding"), "gzip");\n`;
                        headerContent += `  request->send(response);\n`;
                        headerContent += `}\n\n`;

                        if (type === 'html') {
                            const baseName = file.route.replace('.html', '');
                            const funcName = `serve${pascalCase(file.name)}`;

                            // Add both .html and extensionless routes
                            htmlRoutes.push({
                                route: file.route,
                                func: funcName
                            });

                            // Add extensionless route for everything except index and fallback
                            if (!['index', 'fallback'].includes(file.name.toLowerCase().replace('_html', ''))) {
                                htmlRoutes.push({
                                    route: baseName,
                                    func: funcName
                                });
                            }
                        } else {
                            routes.push({
                                route: file.route,
                                func: `serve${pascalCase(file.name)}`
                            });
                        }
                    }

                    await fs.writeFile(resolve(outputDir, headerName), headerContent);
                }
            }

            // Generate routes header
            const routesContent = `#pragma once

#include <ESPAsyncWebServer.h>
#include <Arduino.h>
${Array.from(filesByDir.entries()).flatMap(([dir, files]) => {
    const types = new Set(files.map(f => f.type));
    return Array.from(types).map(type => `#include "ui_${dir}_${type}.h"`);
}).join('\n')}

inline void setupUIRoutes(AsyncWebServer* server) {
${routes.map(r => `    server->on("${r.route}", HTTP_GET, ${r.func});`).join('\n')}

    // HTML routes with and without .html extension
${htmlRoutes.map(r => `    server->on("${r.route}", HTTP_GET, ${r.func});`).join('\n')}

    // Serve index.html for the root path
    server->on("/ui/", HTTP_GET, serveIndexHtml);
}`;
            await fs.writeFile(resolve(outputDir, 'ui_routes.h'), routesContent);
        }
    };
}
