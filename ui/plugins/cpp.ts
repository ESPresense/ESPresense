import type { Plugin } from 'vite';
import { resolve, dirname, relative } from 'path';
import { promisify } from 'util';
import fs from 'fs/promises';
import { pascalCase } from 'change-case';
import mime from 'mime';
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
  content: Buffer;
  contentType: string;
  type: string;
}

interface CompressStats {
  fileName: string;
  inputSize: number;
  compressedSize: number;
  groupName: string;
  useCompression: boolean;
}

interface CppPluginOptions {
  outPrefix?: string;
  basePath?: string;
  /** Final static site directory written by adapter-static. */
  buildDir?: string;
  /** SvelteKit intermediate output root. */
  kitOutDir?: string;
  /** Directory for generated C++ headers. */
  outputDir?: string;
  /** Extensions to skip. */
  skipExtensions?: string[];
}

function formatSize(bytes: number): string {
  if (bytes < 1024) return `${bytes} B`;
  return `${(bytes / 1024).toFixed(2)} kB`;
}

function padEnd(str: string, len: number): string {
  return str.padEnd(len, ' ');
}

function hexdump(buffer: Uint8Array): string {
  const lines: string[] = [];
  for (let i = 0; i < buffer.length; i += 16) {
    const block = buffer.slice(i, i + 16);
    const hexArray = Array.from(block).map(
      (value) => '0x' + value.toString(16).padStart(2, '0')
    );
    lines.push(`  ${hexArray.join(', ')}`);
  }
  return lines.join(',\n');
}

async function compressAsset(
  input: Buffer,
  fileName: string,
  contentType?: string
): Promise<CompressedOutput> {
  const compressed = await zopfliCompress(input, {
    blocksplitting: true,
    blocksplittinglast: false,
    blocksplittingmax: 15,
    verbose: false
  });
  const useCompression = compressed.length < input.length;
  const finalBuffer = useCompression ? compressed : input;

  return {
    name: fileName.replace(/[.-]/g, '_').toUpperCase(),
    length: finalBuffer.length,
    array: hexdump(finalBuffer),
    contentType: contentType || mime.getType(fileName) || 'application/octet-stream',
    useCompression
  };
}

function getGroupName(assetPath: string, type: string, outPrefix: string): string {
  const dir = dirname(assetPath);
  if (dir === '.') return `${outPrefix}${type}`;
  return outPrefix + dir.replace(/\//g, '_') + '_' + type;
}

function generateRoutesForAsset(
  asset: Asset,
  basePath: string
): { routes: string[]; htmlRoutes: string[] } {
  const routes: string[] = [];
  const htmlRoutes: string[] = [];
  const assetName = pascalCase(asset.name);

  if (asset.type === 'html') {
    const routePath =
      asset.path === 'index.html'
        ? basePath + '/'
        : basePath + '/' + asset.path.slice(0, -5);
    htmlRoutes.push(`    server->on("${routePath}", HTTP_GET, serve${assetName});`);
    if (routePath !== basePath + '/') {
      htmlRoutes.push(`    server->on("${routePath}.html", HTTP_GET, serve${assetName});`);
    }
  } else {
    routes.push(`    server->on("${basePath}/${asset.path}", HTTP_GET, serve${assetName});`);
  }

  return { routes, htmlRoutes };
}

async function exists(path: string): Promise<boolean> {
  try {
    await fs.access(path);
    return true;
  } catch {
    return false;
  }
}

function contentTypeFor(fileName: string, ext: string): string {
  if (ext === 'js' || ext === 'mjs') return 'application/javascript';
  if (ext === 'css') return 'text/css';
  if (ext === 'html') return 'text/html';
  return mime.getType(fileName) || 'application/octet-stream';
}

async function collectFiles(
  rootDir: string,
  skipExtensions: Set<string>,
  pathPrefix = ''
): Promise<Asset[]> {
  const assets: Asset[] = [];

  async function walk(dir: string): Promise<void> {
    const entries = await fs.readdir(dir, { withFileTypes: true });
    for (const entry of entries) {
      if (entry.name.startsWith('.')) continue;
      const fullPath = resolve(dir, entry.name);
      if (entry.isDirectory()) {
        await walk(fullPath);
        continue;
      }

      const relFromRoot = relative(rootDir, fullPath).split('\\').join('/');
      const rel = pathPrefix ? `${pathPrefix}/${relFromRoot}` : relFromRoot;
      const ext = (entry.name.split('.').pop() || '').toLowerCase();
      if (skipExtensions.has(ext)) continue;

      const content = await fs.readFile(fullPath);
      assets.push({
        path: rel,
        name: rel.replace(/[\/\\]/g, '_').replace(/[.-]/g, '_'),
        content,
        contentType: contentTypeFor(entry.name, ext),
        type: ext || 'bin'
      });
    }
  }

  if (await exists(rootDir)) {
    await walk(rootDir);
  }
  return assets;
}

async function removeStaleHeaders(
  outputDir: string,
  outPrefix: string,
  keep: Set<string>
): Promise<void> {
  const entries = await fs.readdir(outputDir);
  for (const entry of entries) {
    if (!entry.startsWith(outPrefix) || !entry.endsWith('.h')) continue;
    if (keep.has(entry)) continue;
    await fs.unlink(resolve(outputDir, entry));
    console.log(chalk.yellow(`Removed stale header ${entry}`));
  }
}

export function cppPlugin(options: CppPluginOptions = {}): Plugin {
  const basePath = options.basePath || '';
  const buildDir = resolve(__dirname, options.buildDir || '../build');
  const kitOutDir = resolve(__dirname, options.kitOutDir || '../.svelte-kit/output');
  const outputDir = resolve(__dirname, options.outputDir || '../../src');
  const outPrefix = options.outPrefix || 'web_';
  const skipExtensions = new Set(
    (options.skipExtensions || ['json', 'map', 'txt']).map((e) =>
      e.replace(/^\./, '').toLowerCase()
    )
  );

  // SvelteKit runs client + SSR Vite builds. Emit headers once when the final
  // client assets and prerendered HTML are both available.
  let generated = false;
  let generating: Promise<void> | null = null;

  async function collectFinalAssets(): Promise<Asset[] | null> {
    // Prefer adapter-static output when present (exact flash payload).
    if (
      (await exists(resolve(buildDir, 'index.html'))) &&
      (await exists(resolve(buildDir, 'app/immutable')))
    ) {
      return collectFiles(buildDir, skipExtensions);
    }

    // Fallback: SvelteKit intermediate outputs after prerender.
    const clientDir = resolve(kitOutDir, 'client');
    const prerenderedDir = resolve(kitOutDir, 'prerendered/pages');
    if (!(await exists(resolve(clientDir, 'app/immutable')))) return null;
    if (!(await exists(prerenderedDir))) return null;

    const assets = [
      ...(await collectFiles(clientDir, skipExtensions)),
      ...(await collectFiles(prerenderedDir, skipExtensions))
    ];
    // Dedupe by path (favicon can appear in client output).
    const byPath = new Map<string, Asset>();
    for (const asset of assets) byPath.set(asset.path, asset);
    return Array.from(byPath.values()).sort((a, b) => a.path.localeCompare(b.path));
  }

  async function generateHeaders(): Promise<void> {
    if (generated) return;

    const assets = await collectFinalAssets();
    if (!assets || assets.length === 0) return;

    generated = true;

    const groupedAssets = new Map<string, Asset[]>();
    for (const asset of assets) {
      const groupName = getGroupName(asset.path, asset.type, outPrefix);
      const group = groupedAssets.get(groupName) || [];
      group.push(asset);
      groupedAssets.set(groupName, group);
    }

    await fs.mkdir(outputDir, { recursive: true });

    const routes: string[] = [];
    const htmlRoutes: string[] = [];
    const compressStats: CompressStats[] = [];
    let totalInputSize = 0;
    let totalCompressedSize = 0;
    const writtenHeaders = new Set<string>([`${outPrefix}routes.h`]);

    for (const [groupName, groupAssets] of groupedAssets.entries()) {
      let header = `/*
 * Binary arrays for the Web UI ${groupName} files.
 */

#pragma once
#include <ESPAsyncWebServer.h>
#include <Arduino.h>

`;

      for (const asset of groupAssets) {
        const inputSize = asset.content.length;
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
        header += `  AsyncWebServerResponse *response = request->beginResponse(200, "${asset.contentType}", ${compressed.name}, ${compressed.name}_L);\n`;
        if (asset.path.includes('immutable')) {
          header += `  response->addHeader(F("Cache-Control"), "public, max-age=31536000, immutable");\n`;
        }
        if (compressed.useCompression) {
          header += `  response->addHeader(F("Content-Encoding"), "gzip");\n`;
        }
        header += `  request->send(response);\n`;
        header += `}\n\n`;

        const generatedRoutes = generateRoutesForAsset(asset, basePath);
        routes.push(...generatedRoutes.routes);
        htmlRoutes.push(...generatedRoutes.htmlRoutes);
      }

      const headerName = `${groupName}.h`;
      writtenHeaders.add(headerName);
      await fs.writeFile(resolve(outputDir, headerName), header);
    }

    await removeStaleHeaders(outputDir, outPrefix, writtenHeaders);

    console.log(chalk.cyan('\nGenerating C++ headers for web UI assets:'));
    console.log(chalk.dim('─'.repeat(100)));
    compressStats.sort((a, b) => {
      if (a.groupName === b.groupName) return b.compressedSize - a.compressedSize;
      return a.groupName.localeCompare(b.groupName);
    });

    let currentGroup = '';
    for (const stat of compressStats) {
      if (stat.groupName !== currentGroup) {
        if (currentGroup !== '') console.log('');
        currentGroup = stat.groupName;
      }
      const ratio = stat.inputSize === 0 ? 0 : stat.compressedSize / stat.inputSize;
      const compressionResult = stat.useCompression
        ? chalk.dim('gzip')
        : chalk.yellow('uncompressed');
      console.log(
        chalk.dim(padEnd(stat.fileName, 70)) +
          chalk.blue(padEnd(formatSize(stat.inputSize), 10)) +
          compressionResult +
          ': ' +
          chalk.green(formatSize(stat.compressedSize)) +
          chalk.dim(` (${(ratio * 100).toFixed(1)}%)`)
      );
    }

    console.log(chalk.dim('\n' + '─'.repeat(100)));
    console.log(
      chalk.cyan('Total compressed size: ') +
        chalk.green(formatSize(totalCompressedSize)) +
        chalk.dim(
          ` (${((totalCompressedSize / Math.max(totalInputSize, 1)) * 100).toFixed(1)}% of ${formatSize(totalInputSize)})`
        )
    );
    console.log(chalk.green(`Generated C++ headers in ${outputDir}\n`));

    const groupNames = Array.from(groupedAssets.keys()).sort();
    routes.sort();
    htmlRoutes.sort();

    const groupSizeComments = groupNames.map((groupName) => {
      const totalGroupBytes = compressStats
        .filter((stat) => stat.groupName === groupName)
        .reduce((sum, stat) => sum + stat.compressedSize, 0);
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
${groupNames.map((group) => `#include "${group}.h"`).join('\n')}

inline void setupRoutes(AsyncWebServer* server) {
${routes.join('\n')}
    // HTML routes
${htmlRoutes.join('\n')}
}
`;
    await fs.writeFile(resolve(outputDir, `${outPrefix}routes.h`), routesHeader);
  }

  return {
    name: 'cpp',
    enforce: 'post',
    apply: 'build',
    // Run after SvelteKit's sequential closeBundle (adapter-static write).
    closeBundle: {
      sequential: true,
      order: 'post',
      async handler() {
        if (!generating) {
          generating = generateHeaders().finally(() => {
            generating = null;
          });
        }
        await generating;
      }
    }
  };
}
