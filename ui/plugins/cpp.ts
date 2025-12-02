import type { Plugin } from 'vite';
import { resolve, dirname } from 'path';
import { promisify } from 'util';
import fs from 'fs/promises';
import { pascalCase } from 'change-case';
import mime from 'mime';
import type { OutputBundle } from 'rollup';
import { gzip } from '@gfx/zopfli';
import chalk from 'chalk';

const zopfliCompress = promisify(gzip);

interface CompressedOutput {
  name: string;
  length: number;
  array: string;
  contentType: string;
  useCompression: boolean;
}

interface Asset {
  path: string;
  name: string;
  content: string | Buffer | Uint8Array;
  contentType: string;
  type: string;
  isServer: boolean;
}

interface CompressStats {
  fileName: string;
  inputSize: number;
  compressedSize: number;
  groupName: string;
  useCompression: boolean;
}

interface CppPluginOptions {
  outPrefix?: string;       // Prefix for output files
  basePath?: string;        // Base URL path (e.g., '/ui', '/app', etc.)
  staticDir?: string;       // Directory for static assets
  outputDir?: string;       // Directory for C++ headers
  clientDir?: string;       // Directory for client assets (default: '../.svelte-kit/output/client')
  immutableDir?: string;    // Relative path to immutable assets inside the clientDir (default: 'app/immutable')
}

function formatSize(bytes: number): string {
  if (bytes < 1024) return `${bytes} B`;
  const kb = bytes / 1024;
  return `${kb.toFixed(2)} kB`;
}

function padEnd(str: string, len: number): string {
  return str.padEnd(len, ' ');
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

async function compressAsset(
  input: string | Buffer | Uint8Array,
  fileName: string,
  contentType?: string
): Promise<CompressedOutput> {
  const options = {
    blocksplitting: true,
    blocksplittinglast: false,
    blocksplittingmax: 15,
    verbose: false
  };

  const inputBuffer = Buffer.from(input);
  const compressed = await zopfliCompress(inputBuffer, options);
  const useCompression = compressed.length < inputBuffer.length;
  const finalBuffer = useCompression ? compressed : inputBuffer;

  return {
    name: fileName.replace(/[.-]/g, "_").toUpperCase(),
    length: finalBuffer.length,
    array: hexdump(finalBuffer),
    contentType: contentType || mime.getType(fileName) || 'application/octet-stream',
    useCompression
  };
}

/**
 * Generates a group name based on the asset’s path and file type.
 */
function getGroupName(assetPath: string, type: string, outPrefix: string): string {
  const dir = dirname(assetPath);
  if (dir === '.') return `${outPrefix}${type}`;
  return outPrefix + dir.replace(/\//g, '_') + '_' + type;
}

/**
 * Cleans up the asset path to preserve immutable assets’ directory structure.
 */
function normalizeAssetPath(asset: Asset): string {
  if (asset.path.includes('_app/immutable')) {
    // Remove any client build prefix if present.
    return asset.path.replace('.svelte-kit/output/client/', '');
  }
  return asset.path;
}

/**
 * Given an asset, returns one or more route strings.
 */
function generateRoutesForAsset(
  asset: Asset,
  basePath: string
): { routes: string[]; htmlRoutes: string[] } {
  const routes: string[] = [];
  const htmlRoutes: string[] = [];
  const assetName = pascalCase(asset.name);
  if (asset.type === 'html') {
    // For HTML, serve at both the base route and the file route (except for index.html).
    const routePath =
      asset.path === 'index.html'
        ? basePath + '/'
        : basePath + '/' + asset.path.slice(0, -5); // remove ".html"
    htmlRoutes.push(`    server->on("${routePath}", HTTP_GET, serve${assetName});`);
    if (routePath !== basePath + '/') {
      htmlRoutes.push(`    server->on("${routePath}.html", HTTP_GET, serve${assetName});`);
    }
  } else if (asset.path.includes('_app/immutable')) {
    // For immutable assets, use the exact route.
    const immutablePart = asset.path.split('_app/immutable/')[1];
    const routePath = `${basePath}/_app/immutable/${immutablePart}`;
    routes.push(`    server->on("${routePath}", HTTP_GET, serve${assetName});`);
    // If JS asset, add an alternative non-hashed route.
    if (asset.type === 'js') {
      const baseRoute = routePath.replace(/\.[A-Za-z0-9]+\.js$/, '.js');
      if (baseRoute !== routePath) {
        routes.push(`    server->on("${baseRoute}", HTTP_GET, serve${assetName});`);
      }
    }
  } else {
    // Default case for all other assets.
    routes.push(`    server->on("${basePath}/${asset.path}", HTTP_GET, serve${assetName});`);
  }
  return { routes, htmlRoutes };
}

export function cppPlugin(options: CppPluginOptions = {}): Plugin {
  // Maps to store assets from various sources.
  const staticAssets = new Map<string, Asset>();
  const bundleAssets = new Map<string, Asset>();
  const compressStats: CompressStats[] = [];

  const basePath = options.basePath || '';
  const staticDir = resolve(__dirname, options.staticDir || '../static');
  const outputDir = resolve(__dirname, options.outputDir || '../../src');
  const outPrefix = options.outPrefix || 'web_';

  // Resolve client and immutable directories from options.
  const clientDir = resolve(__dirname, options.clientDir || '../.svelte-kit/output/client');
  const immutableDir = resolve(clientDir, options.immutableDir || 'app/immutable');

  return {
    name: 'cpp',
    enforce: 'post',
    apply: 'build',

    async buildStart() {
      // Clear any previous asset maps.
      staticAssets.clear();
      bundleAssets.clear();

      // Read and cache static assets from the provided static directory.
      try {
        const files = await fs.readdir(staticDir);
        for (const file of files) {
          if (file.startsWith('.')) continue;
          const filePath = resolve(staticDir, file);
          const stats = await fs.stat(filePath);
          if (!stats.isFile()) continue;
          const content = await fs.readFile(filePath);
          const ext = file.split('.').pop()?.toLowerCase() || '';
          staticAssets.set(file, {
            path: file,
            name: file.replace(/[.-]/g, '_'),
            content,
            contentType: mime.getType(file) || 'application/octet-stream',
            type: ext,
            isServer: false
          });
        }
        if (staticAssets.size > 0) {
          console.log(`Captured ${staticAssets.size} static assets from ${staticDir}`);
        }
      } catch (error: any) {
        if (error?.code !== 'ENOENT') {
          console.error(`Error reading static directory ${staticDir}:`, error);
        }
      }
    },

    transformIndexHtml: {
      order: 'post',
      handler(html: string, { filename }) {
        // Skip SSR builds.
        if (filename.includes('.svelte-kit/output/server')) return html;
        const basename = filename.split('/').pop() || filename;
        bundleAssets.set(basename, {
          path: basename,
          name: basename.replace('.html', '_html').replace(/[.-]/g, '_'),
          content: html,
          contentType: 'text/html',
          type: 'html',
          isServer: false
        });
        return html;
      }
    },

    async generateBundle(_, bundle: OutputBundle) {
      // Add non-HTML bundle assets.
      for (const [fileName, file] of Object.entries(bundle)) {
        if (fileName.split('/')[0].startsWith('.')) continue;
        if (fileName.endsWith('.json')) continue;
        if (fileName.endsWith('.html')) continue; // Already handled by transformIndexHtml

        const content = file.type === 'chunk' ? file.code : file.source;
        const ext = fileName.split('.').pop()?.toLowerCase() || '';
        const isServer = fileName.startsWith('.svelte-kit/output/server/') ||
          fileName.includes('/server/') ||
          fileName.includes('entries/');
        bundleAssets.set(fileName, {
          path: fileName,
          name: fileName.replace(/[\/\\]/g, '_').replace(/[.-]/g, '_'),
          content,
          contentType: file.type === 'chunk'
            ? 'application/javascript'
            : mime.getType(fileName) || 'application/octet-stream',
          type: ext,
          isServer
        });
      }
    },

    async closeBundle() {
      // Allow time for any external asset generation (such as adapter-static).
      await new Promise(resolve => setTimeout(resolve, 2000));

      // Attempt to collect additional HTML assets from the build directory.
      const buildDir = resolve(__dirname, '../build');
      try {
        const entries = await fs.readdir(buildDir);
        for (const entry of entries) {
          if (!entry.endsWith('.html')) continue;
          const fullPath = resolve(buildDir, entry);
          const content = await fs.readFile(fullPath, 'utf-8');
          bundleAssets.set(entry, {
            path: entry,
            name: entry.replace(/[.-]/g, '_'),
            content,
            contentType: 'text/html',
            type: 'html',
            isServer: false
          });
        }
      } catch (error: any) {
        if (error?.code !== 'ENOENT') {
          console.error('Error processing HTML files:', error);
        }
      }

      // Process immutable assets from the client directory.
      async function processImmutableDirectory(dir: string, base: string = ''): Promise<void> {
        const entries = await fs.readdir(dir, { withFileTypes: true });
        for (const entry of entries) {
          const fullPath = resolve(dir, entry.name);
          const relativePath = base ? `${base}/${entry.name}` : entry.name;
          if (entry.isDirectory()) {
            await processImmutableDirectory(fullPath, relativePath);
          } else {
            const content = await fs.readFile(fullPath);
            const ext = entry.name.split('.').pop()?.toLowerCase() || '';
            // Prepend the folder structure for immutable assets.
            const assetPath = `${options.immutableDir || 'app/immutable'}/${relativePath}`;
            bundleAssets.set(assetPath, {
              path: assetPath,
              name: assetPath.replace(/[\/\\]/g, '_').replace(/[.-]/g, '_'),
              content,
              contentType: ext === 'js'
                ? 'application/javascript'
                : mime.getType(entry.name) || 'application/octet-stream',
              type: ext,
              isServer: false
            });
          }
        }
      }
      try {
        await processImmutableDirectory(immutableDir);
      } catch (error: any) {
        if (error?.code !== 'ENOENT') {
          console.error('Error processing immutable files:', error);
        }
      }

      // Merge assets from bundle and static sources into groups.
      const groupedAssets = new Map<string, Asset[]>();

      // Process bundle assets (skip server-only assets).
      for (const asset of bundleAssets.values()) {
        if (asset.path.startsWith('.svelte-kit/output/server/')) continue;
        // Only include HTML files, immutable assets, or assets that also exist in staticAssets.
        if (asset.type !== 'html' && !asset.path.includes(options.immutableDir || 'app/immutable') && !staticAssets.has(asset.path)) continue;
        asset.path = normalizeAssetPath(asset);
        const groupName = getGroupName(asset.path, asset.type, outPrefix);
        const group = groupedAssets.get(groupName) || [];
        group.push(asset);
        groupedAssets.set(groupName, group);
      }
      // Add static assets.
      for (const asset of staticAssets.values()) {
        const groupName = getGroupName(asset.path, asset.type, outPrefix);
        const group = groupedAssets.get(groupName) || [];
        group.push(asset);
        groupedAssets.set(groupName, group);
      }

      // Prepare to generate headers.
      try {
        await fs.mkdir(outputDir, { recursive: true });
        const routes: string[] = [];
        const htmlRoutes: string[] = [];
        let totalInputSize = 0;
        let totalCompressedSize = 0;

        // Generate header files for each asset group.
        for (const [groupName, assets] of groupedAssets.entries()) {
          let header = `/*
 * Binary arrays for the Web UI ${groupName} files.
 */

#pragma once
#include <ESPAsyncWebServer.h>
#include <Arduino.h>

`;

          // Process each asset in the group.
          for (const asset of assets) {
            const inputSize = Buffer.from(asset.content).length;
            const compressed = await compressAsset(asset.content, asset.name, asset.contentType);
            compressStats.push({
              fileName: asset.path,
              inputSize,
              compressedSize: compressed.length,
              groupName,
              useCompression: compressed.useCompression
            });
            totalInputSize += inputSize;
            totalCompressedSize += compressed.length;

            header += `// ${asset.path}\n`;
            header += `const uint32_t ${compressed.name}_L = ${compressed.length};\n`;
            header += `const uint8_t ${compressed.name}[] PROGMEM = {\n${compressed.array}\n};\n\n`;
            header += `inline void serve${pascalCase(asset.name)}(AsyncWebServerRequest* request) {\n`;
            header += `  AsyncWebServerResponse *response = request->beginResponse_P(200, "${asset.contentType}", ${compressed.name}, ${compressed.name}_L);\n`;
            if (compressed.useCompression) {
              header += `  response->addHeader(F("Content-Encoding"), "gzip");\n`;
            }
            header += `  request->send(response);\n`;
            header += `}\n\n`;

            // Generate routes for this asset.
            const { routes: assetRoutes, htmlRoutes: assetHtmlRoutes } = generateRoutesForAsset(asset, basePath);
            routes.push(...assetRoutes);
            htmlRoutes.push(...assetHtmlRoutes);
          }

          // Write the group header.
          await fs.writeFile(resolve(outputDir, `${groupName}.h`), header);
        }

        // Output compression stats.
        console.log(chalk.cyan('\nGenerating C++ headers for web UI assets:'));
        console.log(chalk.dim('─'.repeat(100)));
        compressStats.sort((a, b) => {
          if (a.groupName === b.groupName) {
            return b.compressedSize - a.compressedSize;
          }
          return a.groupName.localeCompare(b.groupName);
        });
        let currentGroup = '';
        for (const stat of compressStats) {
          if (stat.groupName !== currentGroup) {
            if (currentGroup !== '') console.log('');
            currentGroup = stat.groupName;
          }
          const ratio = stat.compressedSize / stat.inputSize;
          const compressionResult = stat.useCompression ? chalk.dim('gzip') : chalk.yellow('uncompressed');
          console.log(
            chalk.dim(padEnd(stat.fileName, 70)) +
            chalk.blue(padEnd(formatSize(stat.inputSize), 10)) +
            compressionResult + ': ' +
            chalk.green(formatSize(stat.compressedSize)) +
            chalk.dim(` (${(ratio * 100).toFixed(1)}%)`)
          );
        }
        console.log(chalk.dim('\n' + '─'.repeat(100)));
        console.log(
          chalk.cyan('Total compressed size: ') +
          chalk.green(formatSize(totalCompressedSize)) +
          chalk.dim(` (${((totalCompressedSize / totalInputSize) * 100).toFixed(1)}% of ${formatSize(totalInputSize)})`)
        );
        console.log(chalk.green(`Generated C++ headers in ${outputDir}\n`));

        // Generate the routes header.
        const groupSizeComments = Array.from(groupedAssets.entries()).map(([groupName, assets]) => {
          const groupStats = compressStats.filter(stat => stat.groupName === groupName);
          const totalGroupBytes = groupStats.reduce((sum, stat) => sum + stat.compressedSize, 0);
          return ` * ${groupName}: ${totalGroupBytes.toLocaleString()} bytes`;
        });
        const routesHeader = `/*
 * Web UI Routes
 *
 * Compressed Size Summary:
${groupSizeComments.join('\n')}
 * Total: ${totalCompressedSize.toLocaleString()} bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
${Array.from(groupedAssets.keys()).map(group => `#include "${group}.h"`).join('\n')}

inline void setupRoutes(AsyncWebServer* server) {
${routes.join('\n')}
    // HTML routes
${htmlRoutes.join('\n')}
}
`;
        await fs.writeFile(resolve(outputDir, `${outPrefix}routes.h`), routesHeader);
      } catch (error) {
        console.error(chalk.red(`Error writing output files to ${outputDir}:`), error);
      }
    }
  };
}
