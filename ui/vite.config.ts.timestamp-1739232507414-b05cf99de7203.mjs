// vite.config.ts
import { sveltekit } from "file:///Users/dturner/Code/ESPresense/ESPresense/ui/node_modules/@sveltejs/kit/src/exports/vite/index.js";
import { defineConfig } from "file:///Users/dturner/Code/ESPresense/ESPresense/ui/node_modules/vite/dist/node/index.js";

// plugins/cpp.ts
import { resolve, dirname } from "path";
import { promisify } from "util";
import fs from "fs/promises";
import { pascalCase } from "file:///Users/dturner/Code/ESPresense/ESPresense/ui/node_modules/change-case/dist/index.js";
import mime from "file:///Users/dturner/Code/ESPresense/ESPresense/ui/node_modules/mime/dist/src/index.js";
import { gzip } from "file:///Users/dturner/Code/ESPresense/ESPresense/ui/node_modules/@gfx/zopfli/dist/index.js";
import chalk from "file:///Users/dturner/Code/ESPresense/ESPresense/ui/node_modules/chalk/source/index.js";
var __vite_injected_original_dirname = "/Users/dturner/Code/ESPresense/ESPresense/ui/plugins";
var zopfliCompress = promisify(gzip);
function formatSize(bytes) {
  if (bytes < 1024) return `${bytes} B`;
  const kb = bytes / 1024;
  return `${kb.toFixed(2)} kB`;
}
function padEnd(str, len) {
  return str.padEnd(len, " ");
}
function hexdump(buffer) {
  const lines = [];
  for (let i = 0; i < buffer.length; i += 16) {
    const block = buffer.slice(i, i + 16);
    const hexArray = Array.from(block).map(
      (value) => "0x" + value.toString(16).padStart(2, "0")
    );
    lines.push(`  ${hexArray.join(", ")}`);
  }
  return lines.join(",\n");
}
async function compressAsset(input, fileName, contentType) {
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
    contentType: contentType || mime.getType(fileName) || "application/octet-stream",
    useCompression
  };
}
function getGroupName(assetPath, type, outPrefix) {
  const dir = dirname(assetPath);
  if (dir === ".") return `${outPrefix}${type}`;
  return outPrefix + dir.replace(/\//g, "_") + "_" + type;
}
function normalizeAssetPath(asset) {
  if (asset.path.includes("_app/immutable")) {
    return asset.path.replace(".svelte-kit/output/client/", "");
  }
  return asset.path;
}
function generateRoutesForAsset(asset, basePath) {
  const routes = [];
  const htmlRoutes = [];
  const assetName = pascalCase(asset.name);
  if (asset.type === "html") {
    const routePath = asset.path === "index.html" ? basePath + "/" : basePath + "/" + asset.path.slice(0, -5);
    htmlRoutes.push(`    server->on("${routePath}", HTTP_GET, serve${assetName});`);
    if (routePath !== basePath + "/") {
      htmlRoutes.push(`    server->on("${routePath}.html", HTTP_GET, serve${assetName});`);
    }
  } else if (asset.path.includes("_app/immutable")) {
    const immutablePart = asset.path.split("_app/immutable/")[1];
    const routePath = `${basePath}/_app/immutable/${immutablePart}`;
    routes.push(`    server->on("${routePath}", HTTP_GET, serve${assetName});`);
    if (asset.type === "js") {
      const baseRoute = routePath.replace(/\.[A-Za-z0-9]+\.js$/, ".js");
      if (baseRoute !== routePath) {
        routes.push(`    server->on("${baseRoute}", HTTP_GET, serve${assetName});`);
      }
    }
  } else {
    routes.push(`    server->on("${basePath}/${asset.path}", HTTP_GET, serve${assetName});`);
  }
  return { routes, htmlRoutes };
}
function cppPlugin(options = {}) {
  const staticAssets = /* @__PURE__ */ new Map();
  const bundleAssets = /* @__PURE__ */ new Map();
  const compressStats = [];
  const basePath = options.basePath || "";
  const staticDir = resolve(__vite_injected_original_dirname, options.staticDir || "../static");
  const outputDir = resolve(__vite_injected_original_dirname, options.outputDir || "../../src");
  const outPrefix = options.outPrefix || "web_";
  const clientDir = resolve(__vite_injected_original_dirname, options.clientDir || "../.svelte-kit/output/client");
  const immutableDir = resolve(clientDir, options.immutableDir || "app/immutable");
  return {
    name: "cpp",
    enforce: "post",
    apply: "build",
    async buildStart() {
      staticAssets.clear();
      bundleAssets.clear();
      try {
        const files = await fs.readdir(staticDir);
        for (const file of files) {
          if (file.startsWith(".")) continue;
          const filePath = resolve(staticDir, file);
          const stats = await fs.stat(filePath);
          if (!stats.isFile()) continue;
          const content = await fs.readFile(filePath);
          const ext = file.split(".").pop()?.toLowerCase() || "";
          staticAssets.set(file, {
            path: file,
            name: file.replace(/[.-]/g, "_"),
            content,
            contentType: mime.getType(file) || "application/octet-stream",
            type: ext,
            isServer: false
          });
        }
        if (staticAssets.size > 0) {
          console.log(`Captured ${staticAssets.size} static assets from ${staticDir}`);
        }
      } catch (error) {
        if (error?.code !== "ENOENT") {
          console.error(`Error reading static directory ${staticDir}:`, error);
        }
      }
    },
    transformIndexHtml: {
      order: "post",
      handler(html, { filename }) {
        if (filename.includes(".svelte-kit/output/server")) return html;
        const basename = filename.split("/").pop() || filename;
        bundleAssets.set(basename, {
          path: basename,
          name: basename.replace(".html", "_html").replace(/[.-]/g, "_"),
          content: html,
          contentType: "text/html",
          type: "html",
          isServer: false
        });
        return html;
      }
    },
    async generateBundle(_, bundle) {
      for (const [fileName, file] of Object.entries(bundle)) {
        if (fileName.split("/")[0].startsWith(".")) continue;
        if (fileName.endsWith(".json")) continue;
        if (fileName.endsWith(".html")) continue;
        const content = file.type === "chunk" ? file.code : file.source;
        const ext = fileName.split(".").pop()?.toLowerCase() || "";
        const isServer = fileName.startsWith(".svelte-kit/output/server/") || fileName.includes("/server/") || fileName.includes("entries/");
        bundleAssets.set(fileName, {
          path: fileName,
          name: fileName.replace(/[\/\\]/g, "_").replace(/[.-]/g, "_"),
          content,
          contentType: file.type === "chunk" ? "application/javascript" : mime.getType(fileName) || "application/octet-stream",
          type: ext,
          isServer
        });
      }
    },
    async closeBundle() {
      await new Promise((resolve2) => setTimeout(resolve2, 2e3));
      const buildDir = resolve(__vite_injected_original_dirname, "../build");
      try {
        const entries = await fs.readdir(buildDir);
        for (const entry of entries) {
          if (!entry.endsWith(".html")) continue;
          const fullPath = resolve(buildDir, entry);
          const content = await fs.readFile(fullPath, "utf-8");
          bundleAssets.set(entry, {
            path: entry,
            name: entry.replace(/[.-]/g, "_"),
            content,
            contentType: "text/html",
            type: "html",
            isServer: false
          });
        }
      } catch (error) {
        if (error?.code !== "ENOENT") {
          console.error("Error processing HTML files:", error);
        }
      }
      async function processImmutableDirectory(dir, base = "") {
        const entries = await fs.readdir(dir, { withFileTypes: true });
        for (const entry of entries) {
          const fullPath = resolve(dir, entry.name);
          const relativePath = base ? `${base}/${entry.name}` : entry.name;
          if (entry.isDirectory()) {
            await processImmutableDirectory(fullPath, relativePath);
          } else {
            const content = await fs.readFile(fullPath);
            const ext = entry.name.split(".").pop()?.toLowerCase() || "";
            const assetPath = `${options.immutableDir || "app/immutable"}/${relativePath}`;
            bundleAssets.set(assetPath, {
              path: assetPath,
              name: assetPath.replace(/[\/\\]/g, "_").replace(/[.-]/g, "_"),
              content,
              contentType: ext === "js" ? "application/javascript" : mime.getType(entry.name) || "application/octet-stream",
              type: ext,
              isServer: false
            });
          }
        }
      }
      try {
        await processImmutableDirectory(immutableDir);
      } catch (error) {
        if (error?.code !== "ENOENT") {
          console.error("Error processing immutable files:", error);
        }
      }
      const groupedAssets = /* @__PURE__ */ new Map();
      for (const asset of bundleAssets.values()) {
        if (asset.path.startsWith(".svelte-kit/output/server/")) continue;
        if (asset.type !== "html" && !asset.path.includes(options.immutableDir || "app/immutable") && !staticAssets.has(asset.path)) continue;
        asset.path = normalizeAssetPath(asset);
        const groupName = getGroupName(asset.path, asset.type, outPrefix);
        const group = groupedAssets.get(groupName) || [];
        group.push(asset);
        groupedAssets.set(groupName, group);
      }
      for (const asset of staticAssets.values()) {
        const groupName = getGroupName(asset.path, asset.type, outPrefix);
        const group = groupedAssets.get(groupName) || [];
        group.push(asset);
        groupedAssets.set(groupName, group);
      }
      try {
        await fs.mkdir(outputDir, { recursive: true });
        const routes = [];
        const htmlRoutes = [];
        let totalInputSize = 0;
        let totalCompressedSize = 0;
        for (const [groupName, assets] of groupedAssets.entries()) {
          let header = `/*
 * Binary arrays for the Web UI ${groupName} files.
 */

#pragma once
#include <ESPAsyncWebServer.h>
#include <Arduino.h>

`;
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
            header += `// ${asset.path}
`;
            header += `const uint16_t ${compressed.name}_L = ${compressed.length};
`;
            header += `const uint8_t ${compressed.name}[] PROGMEM = {
${compressed.array}
};

`;
            header += `inline void serve${pascalCase(asset.name)}(AsyncWebServerRequest* request) {
`;
            header += `  AsyncWebServerResponse *response = request->beginResponse_P(200, "${asset.contentType}", ${compressed.name}, ${compressed.name}_L);
`;
            if (compressed.useCompression) {
              header += `  response->addHeader(F("Content-Encoding"), "gzip");
`;
            }
            header += `  request->send(response);
`;
            header += `}

`;
            const { routes: assetRoutes, htmlRoutes: assetHtmlRoutes } = generateRoutesForAsset(asset, basePath);
            routes.push(...assetRoutes);
            htmlRoutes.push(...assetHtmlRoutes);
          }
          await fs.writeFile(resolve(outputDir, `${groupName}.h`), header);
        }
        console.log(chalk.cyan("\nGenerating C++ headers for web UI assets:"));
        console.log(chalk.dim("\u2500".repeat(100)));
        compressStats.sort((a, b) => {
          if (a.groupName === b.groupName) {
            return b.compressedSize - a.compressedSize;
          }
          return a.groupName.localeCompare(b.groupName);
        });
        let currentGroup = "";
        for (const stat of compressStats) {
          if (stat.groupName !== currentGroup) {
            if (currentGroup !== "") console.log("");
            currentGroup = stat.groupName;
          }
          const ratio = stat.compressedSize / stat.inputSize;
          const compressionResult = stat.useCompression ? chalk.dim("gzip") : chalk.yellow("uncompressed");
          console.log(
            chalk.dim(padEnd(stat.fileName, 70)) + chalk.blue(padEnd(formatSize(stat.inputSize), 10)) + compressionResult + ": " + chalk.green(formatSize(stat.compressedSize)) + chalk.dim(` (${(ratio * 100).toFixed(1)}%)`)
          );
        }
        console.log(chalk.dim("\n" + "\u2500".repeat(100)));
        console.log(
          chalk.cyan("Total compressed size: ") + chalk.green(formatSize(totalCompressedSize)) + chalk.dim(` (${(totalCompressedSize / totalInputSize * 100).toFixed(1)}% of ${formatSize(totalInputSize)})`)
        );
        console.log(chalk.green(`Generated C++ headers in ${outputDir}
`));
        const groupSizeComments = Array.from(groupedAssets.entries()).map(([groupName, assets]) => {
          const groupStats = compressStats.filter((stat) => stat.groupName === groupName);
          const totalGroupBytes = groupStats.reduce((sum, stat) => sum + stat.compressedSize, 0);
          return ` * ${groupName}: ${totalGroupBytes.toLocaleString()} bytes`;
        });
        const routesHeader = `/*
 * Web UI Routes
 *
 * Compressed Size Summary:
${groupSizeComments.join("\n")}
 * Total: ${totalCompressedSize.toLocaleString()} bytes
 */

#pragma once

#include <ESPAsyncWebServer.h>
${Array.from(groupedAssets.keys()).map((group) => `#include "${group}.h"`).join("\n")}

inline void setupRoutes(AsyncWebServer* server) {
${routes.join("\n")}
    // HTML routes
${htmlRoutes.join("\n")}
}
`;
        await fs.writeFile(resolve(outputDir, `${outPrefix}routes.h`), routesHeader);
      } catch (error) {
        console.error(chalk.red(`Error writing output files to ${outputDir}:`), error);
      }
    }
  };
}

// vite.config.ts
import strip from "file:///Users/dturner/Code/ESPresense/ESPresense/ui/node_modules/@rollup/plugin-strip/dist/es/index.js";
var vite_config_default = defineConfig({
  plugins: [
    sveltekit(),
    strip({
      include: "**/*.(js|ts|svelte)",
      functions: ["console.*", "assert.*"]
    }),
    cppPlugin({ basePath: "", outPrefix: "ui_" })
  ],
  build: {
    sourcemap: false,
    rollupOptions: {
      output: {
        manualChunks: () => "index"
      }
    }
  },
  server: {
    proxy: {
      "/json": "http://192.168.129.176",
      "/wifi": "http://192.168.129.176",
      "/restart": "http://192.168.129.176",
      "/ws": {
        target: "ws://192.168.129.176",
        ws: true
      }
    }
  }
});
export {
  vite_config_default as default
};
//# sourceMappingURL=data:application/json;base64,ewogICJ2ZXJzaW9uIjogMywKICAic291cmNlcyI6IFsidml0ZS5jb25maWcudHMiLCAicGx1Z2lucy9jcHAudHMiXSwKICAic291cmNlc0NvbnRlbnQiOiBbImNvbnN0IF9fdml0ZV9pbmplY3RlZF9vcmlnaW5hbF9kaXJuYW1lID0gXCIvVXNlcnMvZHR1cm5lci9Db2RlL0VTUHJlc2Vuc2UvRVNQcmVzZW5zZS91aVwiO2NvbnN0IF9fdml0ZV9pbmplY3RlZF9vcmlnaW5hbF9maWxlbmFtZSA9IFwiL1VzZXJzL2R0dXJuZXIvQ29kZS9FU1ByZXNlbnNlL0VTUHJlc2Vuc2UvdWkvdml0ZS5jb25maWcudHNcIjtjb25zdCBfX3ZpdGVfaW5qZWN0ZWRfb3JpZ2luYWxfaW1wb3J0X21ldGFfdXJsID0gXCJmaWxlOi8vL1VzZXJzL2R0dXJuZXIvQ29kZS9FU1ByZXNlbnNlL0VTUHJlc2Vuc2UvdWkvdml0ZS5jb25maWcudHNcIjtpbXBvcnQgeyBzdmVsdGVraXQgfSBmcm9tICdAc3ZlbHRlanMva2l0L3ZpdGUnO1xuaW1wb3J0IHsgZGVmaW5lQ29uZmlnIH0gZnJvbSAndml0ZSc7XG5pbXBvcnQgeyBjcHBQbHVnaW4gfSBmcm9tICcuL3BsdWdpbnMvY3BwJztcbmltcG9ydCBzdHJpcCBmcm9tICdAcm9sbHVwL3BsdWdpbi1zdHJpcCc7XG5cbmV4cG9ydCBkZWZhdWx0IGRlZmluZUNvbmZpZyh7XG4gICAgcGx1Z2luczogW1xuICAgICAgICBzdmVsdGVraXQoKSxcbiAgICAgICAgc3RyaXAoe1xuICAgICAgICAgICAgaW5jbHVkZTogJyoqLyouKGpzfHRzfHN2ZWx0ZSknLFxuICAgICAgICAgICAgZnVuY3Rpb25zOiBbJ2NvbnNvbGUuKicsICdhc3NlcnQuKiddLFxuICAgICAgICB9KSxcbiAgICAgICAgY3BwUGx1Z2luKHsgYmFzZVBhdGg6ICcnLCBvdXRQcmVmaXg6ICd1aV8nIH0pXG4gICAgXSxcbiAgICBidWlsZDoge1xuICAgICAgICBzb3VyY2VtYXA6IGZhbHNlLFxuICAgICAgICByb2xsdXBPcHRpb25zOiB7XG4gICAgICAgICAgICBvdXRwdXQ6IHtcbiAgICAgICAgICAgICAgICBtYW51YWxDaHVua3M6ICgpID0+ICdpbmRleCdcbiAgICAgICAgICAgIH1cbiAgICAgICAgfVxuICAgIH0sXG4gICAgc2VydmVyOiB7XG4gICAgICAgIHByb3h5OiB7XG4gICAgICAgICAgICAnL2pzb24nOiAnaHR0cDovLzE5Mi4xNjguMTI5LjE3NicsXG4gICAgICAgICAgICAnL3dpZmknOiAnaHR0cDovLzE5Mi4xNjguMTI5LjE3NicsXG4gICAgICAgICAgICAnL3Jlc3RhcnQnOiAnaHR0cDovLzE5Mi4xNjguMTI5LjE3NicsXG4gICAgICAgICAgICAnL3dzJzoge1xuICAgICAgICAgICAgICAgIHRhcmdldDogJ3dzOi8vMTkyLjE2OC4xMjkuMTc2JyxcbiAgICAgICAgICAgICAgICB3czogdHJ1ZSxcbiAgICAgICAgICAgIH1cbiAgICAgICAgfVxuICAgIH1cbn0pO1xuIiwgImNvbnN0IF9fdml0ZV9pbmplY3RlZF9vcmlnaW5hbF9kaXJuYW1lID0gXCIvVXNlcnMvZHR1cm5lci9Db2RlL0VTUHJlc2Vuc2UvRVNQcmVzZW5zZS91aS9wbHVnaW5zXCI7Y29uc3QgX192aXRlX2luamVjdGVkX29yaWdpbmFsX2ZpbGVuYW1lID0gXCIvVXNlcnMvZHR1cm5lci9Db2RlL0VTUHJlc2Vuc2UvRVNQcmVzZW5zZS91aS9wbHVnaW5zL2NwcC50c1wiO2NvbnN0IF9fdml0ZV9pbmplY3RlZF9vcmlnaW5hbF9pbXBvcnRfbWV0YV91cmwgPSBcImZpbGU6Ly8vVXNlcnMvZHR1cm5lci9Db2RlL0VTUHJlc2Vuc2UvRVNQcmVzZW5zZS91aS9wbHVnaW5zL2NwcC50c1wiO2ltcG9ydCB0eXBlIHsgUGx1Z2luIH0gZnJvbSAndml0ZSc7XG5pbXBvcnQgeyByZXNvbHZlLCBkaXJuYW1lIH0gZnJvbSAncGF0aCc7XG5pbXBvcnQgeyBwcm9taXNpZnkgfSBmcm9tICd1dGlsJztcbmltcG9ydCBmcyBmcm9tICdmcy9wcm9taXNlcyc7XG5pbXBvcnQgeyBwYXNjYWxDYXNlIH0gZnJvbSAnY2hhbmdlLWNhc2UnO1xuaW1wb3J0IG1pbWUgZnJvbSAnbWltZSc7XG5pbXBvcnQgdHlwZSB7IE91dHB1dEJ1bmRsZSB9IGZyb20gJ3JvbGx1cCc7XG5pbXBvcnQgeyBnemlwIH0gZnJvbSAnQGdmeC96b3BmbGknO1xuaW1wb3J0IGNoYWxrIGZyb20gJ2NoYWxrJztcblxuY29uc3Qgem9wZmxpQ29tcHJlc3MgPSBwcm9taXNpZnkoZ3ppcCk7XG5cbmludGVyZmFjZSBDb21wcmVzc2VkT3V0cHV0IHtcbiAgbmFtZTogc3RyaW5nO1xuICBsZW5ndGg6IG51bWJlcjtcbiAgYXJyYXk6IHN0cmluZztcbiAgY29udGVudFR5cGU6IHN0cmluZztcbiAgdXNlQ29tcHJlc3Npb246IGJvb2xlYW47XG59XG5cbmludGVyZmFjZSBBc3NldCB7XG4gIHBhdGg6IHN0cmluZztcbiAgbmFtZTogc3RyaW5nO1xuICBjb250ZW50OiBzdHJpbmcgfCBCdWZmZXIgfCBVaW50OEFycmF5O1xuICBjb250ZW50VHlwZTogc3RyaW5nO1xuICB0eXBlOiBzdHJpbmc7XG4gIGlzU2VydmVyOiBib29sZWFuO1xufVxuXG5pbnRlcmZhY2UgQ29tcHJlc3NTdGF0cyB7XG4gIGZpbGVOYW1lOiBzdHJpbmc7XG4gIGlucHV0U2l6ZTogbnVtYmVyO1xuICBjb21wcmVzc2VkU2l6ZTogbnVtYmVyO1xuICBncm91cE5hbWU6IHN0cmluZztcbiAgdXNlQ29tcHJlc3Npb246IGJvb2xlYW47XG59XG5cbmludGVyZmFjZSBDcHBQbHVnaW5PcHRpb25zIHtcbiAgb3V0UHJlZml4Pzogc3RyaW5nOyAgICAgICAvLyBQcmVmaXggZm9yIG91dHB1dCBmaWxlc1xuICBiYXNlUGF0aD86IHN0cmluZzsgICAgICAgIC8vIEJhc2UgVVJMIHBhdGggKGUuZy4sICcvdWknLCAnL2FwcCcsIGV0Yy4pXG4gIHN0YXRpY0Rpcj86IHN0cmluZzsgICAgICAgLy8gRGlyZWN0b3J5IGZvciBzdGF0aWMgYXNzZXRzXG4gIG91dHB1dERpcj86IHN0cmluZzsgICAgICAgLy8gRGlyZWN0b3J5IGZvciBDKysgaGVhZGVyc1xuICBjbGllbnREaXI/OiBzdHJpbmc7ICAgICAgIC8vIERpcmVjdG9yeSBmb3IgY2xpZW50IGFzc2V0cyAoZGVmYXVsdDogJy4uLy5zdmVsdGUta2l0L291dHB1dC9jbGllbnQnKVxuICBpbW11dGFibGVEaXI/OiBzdHJpbmc7ICAgIC8vIFJlbGF0aXZlIHBhdGggdG8gaW1tdXRhYmxlIGFzc2V0cyBpbnNpZGUgdGhlIGNsaWVudERpciAoZGVmYXVsdDogJ2FwcC9pbW11dGFibGUnKVxufVxuXG5mdW5jdGlvbiBmb3JtYXRTaXplKGJ5dGVzOiBudW1iZXIpOiBzdHJpbmcge1xuICBpZiAoYnl0ZXMgPCAxMDI0KSByZXR1cm4gYCR7Ynl0ZXN9IEJgO1xuICBjb25zdCBrYiA9IGJ5dGVzIC8gMTAyNDtcbiAgcmV0dXJuIGAke2tiLnRvRml4ZWQoMil9IGtCYDtcbn1cblxuZnVuY3Rpb24gcGFkRW5kKHN0cjogc3RyaW5nLCBsZW46IG51bWJlcik6IHN0cmluZyB7XG4gIHJldHVybiBzdHIucGFkRW5kKGxlbiwgJyAnKTtcbn1cblxuZnVuY3Rpb24gaGV4ZHVtcChidWZmZXI6IFVpbnQ4QXJyYXkpOiBzdHJpbmcge1xuICBjb25zdCBsaW5lczogc3RyaW5nW10gPSBbXTtcbiAgZm9yIChsZXQgaSA9IDA7IGkgPCBidWZmZXIubGVuZ3RoOyBpICs9IDE2KSB7XG4gICAgY29uc3QgYmxvY2sgPSBidWZmZXIuc2xpY2UoaSwgaSArIDE2KTtcbiAgICBjb25zdCBoZXhBcnJheSA9IEFycmF5LmZyb20oYmxvY2spLm1hcCh2YWx1ZSA9PlxuICAgICAgXCIweFwiICsgdmFsdWUudG9TdHJpbmcoMTYpLnBhZFN0YXJ0KDIsIFwiMFwiKVxuICAgICk7XG4gICAgbGluZXMucHVzaChgICAke2hleEFycmF5LmpvaW4oXCIsIFwiKX1gKTtcbiAgfVxuICByZXR1cm4gbGluZXMuam9pbihcIixcXG5cIik7XG59XG5cbmFzeW5jIGZ1bmN0aW9uIGNvbXByZXNzQXNzZXQoXG4gIGlucHV0OiBzdHJpbmcgfCBCdWZmZXIgfCBVaW50OEFycmF5LFxuICBmaWxlTmFtZTogc3RyaW5nLFxuICBjb250ZW50VHlwZT86IHN0cmluZ1xuKTogUHJvbWlzZTxDb21wcmVzc2VkT3V0cHV0PiB7XG4gIGNvbnN0IG9wdGlvbnMgPSB7XG4gICAgYmxvY2tzcGxpdHRpbmc6IHRydWUsXG4gICAgYmxvY2tzcGxpdHRpbmdsYXN0OiBmYWxzZSxcbiAgICBibG9ja3NwbGl0dGluZ21heDogMTUsXG4gICAgdmVyYm9zZTogZmFsc2VcbiAgfTtcblxuICBjb25zdCBpbnB1dEJ1ZmZlciA9IEJ1ZmZlci5mcm9tKGlucHV0KTtcbiAgY29uc3QgY29tcHJlc3NlZCA9IGF3YWl0IHpvcGZsaUNvbXByZXNzKGlucHV0QnVmZmVyLCBvcHRpb25zKTtcbiAgY29uc3QgdXNlQ29tcHJlc3Npb24gPSBjb21wcmVzc2VkLmxlbmd0aCA8IGlucHV0QnVmZmVyLmxlbmd0aDtcbiAgY29uc3QgZmluYWxCdWZmZXIgPSB1c2VDb21wcmVzc2lvbiA/IGNvbXByZXNzZWQgOiBpbnB1dEJ1ZmZlcjtcblxuICByZXR1cm4ge1xuICAgIG5hbWU6IGZpbGVOYW1lLnJlcGxhY2UoL1suLV0vZywgXCJfXCIpLnRvVXBwZXJDYXNlKCksXG4gICAgbGVuZ3RoOiBmaW5hbEJ1ZmZlci5sZW5ndGgsXG4gICAgYXJyYXk6IGhleGR1bXAoZmluYWxCdWZmZXIpLFxuICAgIGNvbnRlbnRUeXBlOiBjb250ZW50VHlwZSB8fCBtaW1lLmdldFR5cGUoZmlsZU5hbWUpIHx8ICdhcHBsaWNhdGlvbi9vY3RldC1zdHJlYW0nLFxuICAgIHVzZUNvbXByZXNzaW9uXG4gIH07XG59XG5cbi8qKlxuICogR2VuZXJhdGVzIGEgZ3JvdXAgbmFtZSBiYXNlZCBvbiB0aGUgYXNzZXRcdTIwMTlzIHBhdGggYW5kIGZpbGUgdHlwZS5cbiAqL1xuZnVuY3Rpb24gZ2V0R3JvdXBOYW1lKGFzc2V0UGF0aDogc3RyaW5nLCB0eXBlOiBzdHJpbmcsIG91dFByZWZpeDogc3RyaW5nKTogc3RyaW5nIHtcbiAgY29uc3QgZGlyID0gZGlybmFtZShhc3NldFBhdGgpO1xuICBpZiAoZGlyID09PSAnLicpIHJldHVybiBgJHtvdXRQcmVmaXh9JHt0eXBlfWA7XG4gIHJldHVybiBvdXRQcmVmaXggKyBkaXIucmVwbGFjZSgvXFwvL2csICdfJykgKyAnXycgKyB0eXBlO1xufVxuXG4vKipcbiAqIENsZWFucyB1cCB0aGUgYXNzZXQgcGF0aCB0byBwcmVzZXJ2ZSBpbW11dGFibGUgYXNzZXRzXHUyMDE5IGRpcmVjdG9yeSBzdHJ1Y3R1cmUuXG4gKi9cbmZ1bmN0aW9uIG5vcm1hbGl6ZUFzc2V0UGF0aChhc3NldDogQXNzZXQpOiBzdHJpbmcge1xuICBpZiAoYXNzZXQucGF0aC5pbmNsdWRlcygnX2FwcC9pbW11dGFibGUnKSkge1xuICAgIC8vIFJlbW92ZSBhbnkgY2xpZW50IGJ1aWxkIHByZWZpeCBpZiBwcmVzZW50LlxuICAgIHJldHVybiBhc3NldC5wYXRoLnJlcGxhY2UoJy5zdmVsdGUta2l0L291dHB1dC9jbGllbnQvJywgJycpO1xuICB9XG4gIHJldHVybiBhc3NldC5wYXRoO1xufVxuXG4vKipcbiAqIEdpdmVuIGFuIGFzc2V0LCByZXR1cm5zIG9uZSBvciBtb3JlIHJvdXRlIHN0cmluZ3MuXG4gKi9cbmZ1bmN0aW9uIGdlbmVyYXRlUm91dGVzRm9yQXNzZXQoXG4gIGFzc2V0OiBBc3NldCxcbiAgYmFzZVBhdGg6IHN0cmluZ1xuKTogeyByb3V0ZXM6IHN0cmluZ1tdOyBodG1sUm91dGVzOiBzdHJpbmdbXSB9IHtcbiAgY29uc3Qgcm91dGVzOiBzdHJpbmdbXSA9IFtdO1xuICBjb25zdCBodG1sUm91dGVzOiBzdHJpbmdbXSA9IFtdO1xuICBjb25zdCBhc3NldE5hbWUgPSBwYXNjYWxDYXNlKGFzc2V0Lm5hbWUpO1xuICBpZiAoYXNzZXQudHlwZSA9PT0gJ2h0bWwnKSB7XG4gICAgLy8gRm9yIEhUTUwsIHNlcnZlIGF0IGJvdGggdGhlIGJhc2Ugcm91dGUgYW5kIHRoZSBmaWxlIHJvdXRlIChleGNlcHQgZm9yIGluZGV4Lmh0bWwpLlxuICAgIGNvbnN0IHJvdXRlUGF0aCA9XG4gICAgICBhc3NldC5wYXRoID09PSAnaW5kZXguaHRtbCdcbiAgICAgICAgPyBiYXNlUGF0aCArICcvJ1xuICAgICAgICA6IGJhc2VQYXRoICsgJy8nICsgYXNzZXQucGF0aC5zbGljZSgwLCAtNSk7IC8vIHJlbW92ZSBcIi5odG1sXCJcbiAgICBodG1sUm91dGVzLnB1c2goYCAgICBzZXJ2ZXItPm9uKFwiJHtyb3V0ZVBhdGh9XCIsIEhUVFBfR0VULCBzZXJ2ZSR7YXNzZXROYW1lfSk7YCk7XG4gICAgaWYgKHJvdXRlUGF0aCAhPT0gYmFzZVBhdGggKyAnLycpIHtcbiAgICAgIGh0bWxSb3V0ZXMucHVzaChgICAgIHNlcnZlci0+b24oXCIke3JvdXRlUGF0aH0uaHRtbFwiLCBIVFRQX0dFVCwgc2VydmUke2Fzc2V0TmFtZX0pO2ApO1xuICAgIH1cbiAgfSBlbHNlIGlmIChhc3NldC5wYXRoLmluY2x1ZGVzKCdfYXBwL2ltbXV0YWJsZScpKSB7XG4gICAgLy8gRm9yIGltbXV0YWJsZSBhc3NldHMsIHVzZSB0aGUgZXhhY3Qgcm91dGUuXG4gICAgY29uc3QgaW1tdXRhYmxlUGFydCA9IGFzc2V0LnBhdGguc3BsaXQoJ19hcHAvaW1tdXRhYmxlLycpWzFdO1xuICAgIGNvbnN0IHJvdXRlUGF0aCA9IGAke2Jhc2VQYXRofS9fYXBwL2ltbXV0YWJsZS8ke2ltbXV0YWJsZVBhcnR9YDtcbiAgICByb3V0ZXMucHVzaChgICAgIHNlcnZlci0+b24oXCIke3JvdXRlUGF0aH1cIiwgSFRUUF9HRVQsIHNlcnZlJHthc3NldE5hbWV9KTtgKTtcbiAgICAvLyBJZiBKUyBhc3NldCwgYWRkIGFuIGFsdGVybmF0aXZlIG5vbi1oYXNoZWQgcm91dGUuXG4gICAgaWYgKGFzc2V0LnR5cGUgPT09ICdqcycpIHtcbiAgICAgIGNvbnN0IGJhc2VSb3V0ZSA9IHJvdXRlUGF0aC5yZXBsYWNlKC9cXC5bQS1aYS16MC05XStcXC5qcyQvLCAnLmpzJyk7XG4gICAgICBpZiAoYmFzZVJvdXRlICE9PSByb3V0ZVBhdGgpIHtcbiAgICAgICAgcm91dGVzLnB1c2goYCAgICBzZXJ2ZXItPm9uKFwiJHtiYXNlUm91dGV9XCIsIEhUVFBfR0VULCBzZXJ2ZSR7YXNzZXROYW1lfSk7YCk7XG4gICAgICB9XG4gICAgfVxuICB9IGVsc2Uge1xuICAgIC8vIERlZmF1bHQgY2FzZSBmb3IgYWxsIG90aGVyIGFzc2V0cy5cbiAgICByb3V0ZXMucHVzaChgICAgIHNlcnZlci0+b24oXCIke2Jhc2VQYXRofS8ke2Fzc2V0LnBhdGh9XCIsIEhUVFBfR0VULCBzZXJ2ZSR7YXNzZXROYW1lfSk7YCk7XG4gIH1cbiAgcmV0dXJuIHsgcm91dGVzLCBodG1sUm91dGVzIH07XG59XG5cbmV4cG9ydCBmdW5jdGlvbiBjcHBQbHVnaW4ob3B0aW9uczogQ3BwUGx1Z2luT3B0aW9ucyA9IHt9KTogUGx1Z2luIHtcbiAgLy8gTWFwcyB0byBzdG9yZSBhc3NldHMgZnJvbSB2YXJpb3VzIHNvdXJjZXMuXG4gIGNvbnN0IHN0YXRpY0Fzc2V0cyA9IG5ldyBNYXA8c3RyaW5nLCBBc3NldD4oKTtcbiAgY29uc3QgYnVuZGxlQXNzZXRzID0gbmV3IE1hcDxzdHJpbmcsIEFzc2V0PigpO1xuICBjb25zdCBjb21wcmVzc1N0YXRzOiBDb21wcmVzc1N0YXRzW10gPSBbXTtcblxuICBjb25zdCBiYXNlUGF0aCA9IG9wdGlvbnMuYmFzZVBhdGggfHwgJyc7XG4gIGNvbnN0IHN0YXRpY0RpciA9IHJlc29sdmUoX19kaXJuYW1lLCBvcHRpb25zLnN0YXRpY0RpciB8fCAnLi4vc3RhdGljJyk7XG4gIGNvbnN0IG91dHB1dERpciA9IHJlc29sdmUoX19kaXJuYW1lLCBvcHRpb25zLm91dHB1dERpciB8fCAnLi4vLi4vc3JjJyk7XG4gIGNvbnN0IG91dFByZWZpeCA9IG9wdGlvbnMub3V0UHJlZml4IHx8ICd3ZWJfJztcblxuICAvLyBSZXNvbHZlIGNsaWVudCBhbmQgaW1tdXRhYmxlIGRpcmVjdG9yaWVzIGZyb20gb3B0aW9ucy5cbiAgY29uc3QgY2xpZW50RGlyID0gcmVzb2x2ZShfX2Rpcm5hbWUsIG9wdGlvbnMuY2xpZW50RGlyIHx8ICcuLi8uc3ZlbHRlLWtpdC9vdXRwdXQvY2xpZW50Jyk7XG4gIGNvbnN0IGltbXV0YWJsZURpciA9IHJlc29sdmUoY2xpZW50RGlyLCBvcHRpb25zLmltbXV0YWJsZURpciB8fCAnYXBwL2ltbXV0YWJsZScpO1xuXG4gIHJldHVybiB7XG4gICAgbmFtZTogJ2NwcCcsXG4gICAgZW5mb3JjZTogJ3Bvc3QnLFxuICAgIGFwcGx5OiAnYnVpbGQnLFxuXG4gICAgYXN5bmMgYnVpbGRTdGFydCgpIHtcbiAgICAgIC8vIENsZWFyIGFueSBwcmV2aW91cyBhc3NldCBtYXBzLlxuICAgICAgc3RhdGljQXNzZXRzLmNsZWFyKCk7XG4gICAgICBidW5kbGVBc3NldHMuY2xlYXIoKTtcblxuICAgICAgLy8gUmVhZCBhbmQgY2FjaGUgc3RhdGljIGFzc2V0cyBmcm9tIHRoZSBwcm92aWRlZCBzdGF0aWMgZGlyZWN0b3J5LlxuICAgICAgdHJ5IHtcbiAgICAgICAgY29uc3QgZmlsZXMgPSBhd2FpdCBmcy5yZWFkZGlyKHN0YXRpY0Rpcik7XG4gICAgICAgIGZvciAoY29uc3QgZmlsZSBvZiBmaWxlcykge1xuICAgICAgICAgIGlmIChmaWxlLnN0YXJ0c1dpdGgoJy4nKSkgY29udGludWU7XG4gICAgICAgICAgY29uc3QgZmlsZVBhdGggPSByZXNvbHZlKHN0YXRpY0RpciwgZmlsZSk7XG4gICAgICAgICAgY29uc3Qgc3RhdHMgPSBhd2FpdCBmcy5zdGF0KGZpbGVQYXRoKTtcbiAgICAgICAgICBpZiAoIXN0YXRzLmlzRmlsZSgpKSBjb250aW51ZTtcbiAgICAgICAgICBjb25zdCBjb250ZW50ID0gYXdhaXQgZnMucmVhZEZpbGUoZmlsZVBhdGgpO1xuICAgICAgICAgIGNvbnN0IGV4dCA9IGZpbGUuc3BsaXQoJy4nKS5wb3AoKT8udG9Mb3dlckNhc2UoKSB8fCAnJztcbiAgICAgICAgICBzdGF0aWNBc3NldHMuc2V0KGZpbGUsIHtcbiAgICAgICAgICAgIHBhdGg6IGZpbGUsXG4gICAgICAgICAgICBuYW1lOiBmaWxlLnJlcGxhY2UoL1suLV0vZywgJ18nKSxcbiAgICAgICAgICAgIGNvbnRlbnQsXG4gICAgICAgICAgICBjb250ZW50VHlwZTogbWltZS5nZXRUeXBlKGZpbGUpIHx8ICdhcHBsaWNhdGlvbi9vY3RldC1zdHJlYW0nLFxuICAgICAgICAgICAgdHlwZTogZXh0LFxuICAgICAgICAgICAgaXNTZXJ2ZXI6IGZhbHNlXG4gICAgICAgICAgfSk7XG4gICAgICAgIH1cbiAgICAgICAgaWYgKHN0YXRpY0Fzc2V0cy5zaXplID4gMCkge1xuICAgICAgICAgIGNvbnNvbGUubG9nKGBDYXB0dXJlZCAke3N0YXRpY0Fzc2V0cy5zaXplfSBzdGF0aWMgYXNzZXRzIGZyb20gJHtzdGF0aWNEaXJ9YCk7XG4gICAgICAgIH1cbiAgICAgIH0gY2F0Y2ggKGVycm9yOiBhbnkpIHtcbiAgICAgICAgaWYgKGVycm9yPy5jb2RlICE9PSAnRU5PRU5UJykge1xuICAgICAgICAgIGNvbnNvbGUuZXJyb3IoYEVycm9yIHJlYWRpbmcgc3RhdGljIGRpcmVjdG9yeSAke3N0YXRpY0Rpcn06YCwgZXJyb3IpO1xuICAgICAgICB9XG4gICAgICB9XG4gICAgfSxcblxuICAgIHRyYW5zZm9ybUluZGV4SHRtbDoge1xuICAgICAgb3JkZXI6ICdwb3N0JyxcbiAgICAgIGhhbmRsZXIoaHRtbDogc3RyaW5nLCB7IGZpbGVuYW1lIH0pIHtcbiAgICAgICAgLy8gU2tpcCBTU1IgYnVpbGRzLlxuICAgICAgICBpZiAoZmlsZW5hbWUuaW5jbHVkZXMoJy5zdmVsdGUta2l0L291dHB1dC9zZXJ2ZXInKSkgcmV0dXJuIGh0bWw7XG4gICAgICAgIGNvbnN0IGJhc2VuYW1lID0gZmlsZW5hbWUuc3BsaXQoJy8nKS5wb3AoKSB8fCBmaWxlbmFtZTtcbiAgICAgICAgYnVuZGxlQXNzZXRzLnNldChiYXNlbmFtZSwge1xuICAgICAgICAgIHBhdGg6IGJhc2VuYW1lLFxuICAgICAgICAgIG5hbWU6IGJhc2VuYW1lLnJlcGxhY2UoJy5odG1sJywgJ19odG1sJykucmVwbGFjZSgvWy4tXS9nLCAnXycpLFxuICAgICAgICAgIGNvbnRlbnQ6IGh0bWwsXG4gICAgICAgICAgY29udGVudFR5cGU6ICd0ZXh0L2h0bWwnLFxuICAgICAgICAgIHR5cGU6ICdodG1sJyxcbiAgICAgICAgICBpc1NlcnZlcjogZmFsc2VcbiAgICAgICAgfSk7XG4gICAgICAgIHJldHVybiBodG1sO1xuICAgICAgfVxuICAgIH0sXG5cbiAgICBhc3luYyBnZW5lcmF0ZUJ1bmRsZShfLCBidW5kbGU6IE91dHB1dEJ1bmRsZSkge1xuICAgICAgLy8gQWRkIG5vbi1IVE1MIGJ1bmRsZSBhc3NldHMuXG4gICAgICBmb3IgKGNvbnN0IFtmaWxlTmFtZSwgZmlsZV0gb2YgT2JqZWN0LmVudHJpZXMoYnVuZGxlKSkge1xuICAgICAgICBpZiAoZmlsZU5hbWUuc3BsaXQoJy8nKVswXS5zdGFydHNXaXRoKCcuJykpIGNvbnRpbnVlO1xuICAgICAgICBpZiAoZmlsZU5hbWUuZW5kc1dpdGgoJy5qc29uJykpIGNvbnRpbnVlO1xuICAgICAgICBpZiAoZmlsZU5hbWUuZW5kc1dpdGgoJy5odG1sJykpIGNvbnRpbnVlOyAvLyBBbHJlYWR5IGhhbmRsZWQgYnkgdHJhbnNmb3JtSW5kZXhIdG1sXG5cbiAgICAgICAgY29uc3QgY29udGVudCA9IGZpbGUudHlwZSA9PT0gJ2NodW5rJyA/IGZpbGUuY29kZSA6IGZpbGUuc291cmNlO1xuICAgICAgICBjb25zdCBleHQgPSBmaWxlTmFtZS5zcGxpdCgnLicpLnBvcCgpPy50b0xvd2VyQ2FzZSgpIHx8ICcnO1xuICAgICAgICBjb25zdCBpc1NlcnZlciA9IGZpbGVOYW1lLnN0YXJ0c1dpdGgoJy5zdmVsdGUta2l0L291dHB1dC9zZXJ2ZXIvJykgfHxcbiAgICAgICAgICBmaWxlTmFtZS5pbmNsdWRlcygnL3NlcnZlci8nKSB8fFxuICAgICAgICAgIGZpbGVOYW1lLmluY2x1ZGVzKCdlbnRyaWVzLycpO1xuICAgICAgICBidW5kbGVBc3NldHMuc2V0KGZpbGVOYW1lLCB7XG4gICAgICAgICAgcGF0aDogZmlsZU5hbWUsXG4gICAgICAgICAgbmFtZTogZmlsZU5hbWUucmVwbGFjZSgvW1xcL1xcXFxdL2csICdfJykucmVwbGFjZSgvWy4tXS9nLCAnXycpLFxuICAgICAgICAgIGNvbnRlbnQsXG4gICAgICAgICAgY29udGVudFR5cGU6IGZpbGUudHlwZSA9PT0gJ2NodW5rJ1xuICAgICAgICAgICAgPyAnYXBwbGljYXRpb24vamF2YXNjcmlwdCdcbiAgICAgICAgICAgIDogbWltZS5nZXRUeXBlKGZpbGVOYW1lKSB8fCAnYXBwbGljYXRpb24vb2N0ZXQtc3RyZWFtJyxcbiAgICAgICAgICB0eXBlOiBleHQsXG4gICAgICAgICAgaXNTZXJ2ZXJcbiAgICAgICAgfSk7XG4gICAgICB9XG4gICAgfSxcblxuICAgIGFzeW5jIGNsb3NlQnVuZGxlKCkge1xuICAgICAgLy8gQWxsb3cgdGltZSBmb3IgYW55IGV4dGVybmFsIGFzc2V0IGdlbmVyYXRpb24gKHN1Y2ggYXMgYWRhcHRlci1zdGF0aWMpLlxuICAgICAgYXdhaXQgbmV3IFByb21pc2UocmVzb2x2ZSA9PiBzZXRUaW1lb3V0KHJlc29sdmUsIDIwMDApKTtcblxuICAgICAgLy8gQXR0ZW1wdCB0byBjb2xsZWN0IGFkZGl0aW9uYWwgSFRNTCBhc3NldHMgZnJvbSB0aGUgYnVpbGQgZGlyZWN0b3J5LlxuICAgICAgY29uc3QgYnVpbGREaXIgPSByZXNvbHZlKF9fZGlybmFtZSwgJy4uL2J1aWxkJyk7XG4gICAgICB0cnkge1xuICAgICAgICBjb25zdCBlbnRyaWVzID0gYXdhaXQgZnMucmVhZGRpcihidWlsZERpcik7XG4gICAgICAgIGZvciAoY29uc3QgZW50cnkgb2YgZW50cmllcykge1xuICAgICAgICAgIGlmICghZW50cnkuZW5kc1dpdGgoJy5odG1sJykpIGNvbnRpbnVlO1xuICAgICAgICAgIGNvbnN0IGZ1bGxQYXRoID0gcmVzb2x2ZShidWlsZERpciwgZW50cnkpO1xuICAgICAgICAgIGNvbnN0IGNvbnRlbnQgPSBhd2FpdCBmcy5yZWFkRmlsZShmdWxsUGF0aCwgJ3V0Zi04Jyk7XG4gICAgICAgICAgYnVuZGxlQXNzZXRzLnNldChlbnRyeSwge1xuICAgICAgICAgICAgcGF0aDogZW50cnksXG4gICAgICAgICAgICBuYW1lOiBlbnRyeS5yZXBsYWNlKC9bLi1dL2csICdfJyksXG4gICAgICAgICAgICBjb250ZW50LFxuICAgICAgICAgICAgY29udGVudFR5cGU6ICd0ZXh0L2h0bWwnLFxuICAgICAgICAgICAgdHlwZTogJ2h0bWwnLFxuICAgICAgICAgICAgaXNTZXJ2ZXI6IGZhbHNlXG4gICAgICAgICAgfSk7XG4gICAgICAgIH1cbiAgICAgIH0gY2F0Y2ggKGVycm9yOiBhbnkpIHtcbiAgICAgICAgaWYgKGVycm9yPy5jb2RlICE9PSAnRU5PRU5UJykge1xuICAgICAgICAgIGNvbnNvbGUuZXJyb3IoJ0Vycm9yIHByb2Nlc3NpbmcgSFRNTCBmaWxlczonLCBlcnJvcik7XG4gICAgICAgIH1cbiAgICAgIH1cblxuICAgICAgLy8gUHJvY2VzcyBpbW11dGFibGUgYXNzZXRzIGZyb20gdGhlIGNsaWVudCBkaXJlY3RvcnkuXG4gICAgICBhc3luYyBmdW5jdGlvbiBwcm9jZXNzSW1tdXRhYmxlRGlyZWN0b3J5KGRpcjogc3RyaW5nLCBiYXNlOiBzdHJpbmcgPSAnJyk6IFByb21pc2U8dm9pZD4ge1xuICAgICAgICBjb25zdCBlbnRyaWVzID0gYXdhaXQgZnMucmVhZGRpcihkaXIsIHsgd2l0aEZpbGVUeXBlczogdHJ1ZSB9KTtcbiAgICAgICAgZm9yIChjb25zdCBlbnRyeSBvZiBlbnRyaWVzKSB7XG4gICAgICAgICAgY29uc3QgZnVsbFBhdGggPSByZXNvbHZlKGRpciwgZW50cnkubmFtZSk7XG4gICAgICAgICAgY29uc3QgcmVsYXRpdmVQYXRoID0gYmFzZSA/IGAke2Jhc2V9LyR7ZW50cnkubmFtZX1gIDogZW50cnkubmFtZTtcbiAgICAgICAgICBpZiAoZW50cnkuaXNEaXJlY3RvcnkoKSkge1xuICAgICAgICAgICAgYXdhaXQgcHJvY2Vzc0ltbXV0YWJsZURpcmVjdG9yeShmdWxsUGF0aCwgcmVsYXRpdmVQYXRoKTtcbiAgICAgICAgICB9IGVsc2Uge1xuICAgICAgICAgICAgY29uc3QgY29udGVudCA9IGF3YWl0IGZzLnJlYWRGaWxlKGZ1bGxQYXRoKTtcbiAgICAgICAgICAgIGNvbnN0IGV4dCA9IGVudHJ5Lm5hbWUuc3BsaXQoJy4nKS5wb3AoKT8udG9Mb3dlckNhc2UoKSB8fCAnJztcbiAgICAgICAgICAgIC8vIFByZXBlbmQgdGhlIGZvbGRlciBzdHJ1Y3R1cmUgZm9yIGltbXV0YWJsZSBhc3NldHMuXG4gICAgICAgICAgICBjb25zdCBhc3NldFBhdGggPSBgJHtvcHRpb25zLmltbXV0YWJsZURpciB8fCAnYXBwL2ltbXV0YWJsZSd9LyR7cmVsYXRpdmVQYXRofWA7XG4gICAgICAgICAgICBidW5kbGVBc3NldHMuc2V0KGFzc2V0UGF0aCwge1xuICAgICAgICAgICAgICBwYXRoOiBhc3NldFBhdGgsXG4gICAgICAgICAgICAgIG5hbWU6IGFzc2V0UGF0aC5yZXBsYWNlKC9bXFwvXFxcXF0vZywgJ18nKS5yZXBsYWNlKC9bLi1dL2csICdfJyksXG4gICAgICAgICAgICAgIGNvbnRlbnQsXG4gICAgICAgICAgICAgIGNvbnRlbnRUeXBlOiBleHQgPT09ICdqcydcbiAgICAgICAgICAgICAgICA/ICdhcHBsaWNhdGlvbi9qYXZhc2NyaXB0J1xuICAgICAgICAgICAgICAgIDogbWltZS5nZXRUeXBlKGVudHJ5Lm5hbWUpIHx8ICdhcHBsaWNhdGlvbi9vY3RldC1zdHJlYW0nLFxuICAgICAgICAgICAgICB0eXBlOiBleHQsXG4gICAgICAgICAgICAgIGlzU2VydmVyOiBmYWxzZVxuICAgICAgICAgICAgfSk7XG4gICAgICAgICAgfVxuICAgICAgICB9XG4gICAgICB9XG4gICAgICB0cnkge1xuICAgICAgICBhd2FpdCBwcm9jZXNzSW1tdXRhYmxlRGlyZWN0b3J5KGltbXV0YWJsZURpcik7XG4gICAgICB9IGNhdGNoIChlcnJvcjogYW55KSB7XG4gICAgICAgIGlmIChlcnJvcj8uY29kZSAhPT0gJ0VOT0VOVCcpIHtcbiAgICAgICAgICBjb25zb2xlLmVycm9yKCdFcnJvciBwcm9jZXNzaW5nIGltbXV0YWJsZSBmaWxlczonLCBlcnJvcik7XG4gICAgICAgIH1cbiAgICAgIH1cblxuICAgICAgLy8gTWVyZ2UgYXNzZXRzIGZyb20gYnVuZGxlIGFuZCBzdGF0aWMgc291cmNlcyBpbnRvIGdyb3Vwcy5cbiAgICAgIGNvbnN0IGdyb3VwZWRBc3NldHMgPSBuZXcgTWFwPHN0cmluZywgQXNzZXRbXT4oKTtcblxuICAgICAgLy8gUHJvY2VzcyBidW5kbGUgYXNzZXRzIChza2lwIHNlcnZlci1vbmx5IGFzc2V0cykuXG4gICAgICBmb3IgKGNvbnN0IGFzc2V0IG9mIGJ1bmRsZUFzc2V0cy52YWx1ZXMoKSkge1xuICAgICAgICBpZiAoYXNzZXQucGF0aC5zdGFydHNXaXRoKCcuc3ZlbHRlLWtpdC9vdXRwdXQvc2VydmVyLycpKSBjb250aW51ZTtcbiAgICAgICAgLy8gT25seSBpbmNsdWRlIEhUTUwgZmlsZXMsIGltbXV0YWJsZSBhc3NldHMsIG9yIGFzc2V0cyB0aGF0IGFsc28gZXhpc3QgaW4gc3RhdGljQXNzZXRzLlxuICAgICAgICBpZiAoYXNzZXQudHlwZSAhPT0gJ2h0bWwnICYmICFhc3NldC5wYXRoLmluY2x1ZGVzKG9wdGlvbnMuaW1tdXRhYmxlRGlyIHx8ICdhcHAvaW1tdXRhYmxlJykgJiYgIXN0YXRpY0Fzc2V0cy5oYXMoYXNzZXQucGF0aCkpIGNvbnRpbnVlO1xuICAgICAgICBhc3NldC5wYXRoID0gbm9ybWFsaXplQXNzZXRQYXRoKGFzc2V0KTtcbiAgICAgICAgY29uc3QgZ3JvdXBOYW1lID0gZ2V0R3JvdXBOYW1lKGFzc2V0LnBhdGgsIGFzc2V0LnR5cGUsIG91dFByZWZpeCk7XG4gICAgICAgIGNvbnN0IGdyb3VwID0gZ3JvdXBlZEFzc2V0cy5nZXQoZ3JvdXBOYW1lKSB8fCBbXTtcbiAgICAgICAgZ3JvdXAucHVzaChhc3NldCk7XG4gICAgICAgIGdyb3VwZWRBc3NldHMuc2V0KGdyb3VwTmFtZSwgZ3JvdXApO1xuICAgICAgfVxuICAgICAgLy8gQWRkIHN0YXRpYyBhc3NldHMuXG4gICAgICBmb3IgKGNvbnN0IGFzc2V0IG9mIHN0YXRpY0Fzc2V0cy52YWx1ZXMoKSkge1xuICAgICAgICBjb25zdCBncm91cE5hbWUgPSBnZXRHcm91cE5hbWUoYXNzZXQucGF0aCwgYXNzZXQudHlwZSwgb3V0UHJlZml4KTtcbiAgICAgICAgY29uc3QgZ3JvdXAgPSBncm91cGVkQXNzZXRzLmdldChncm91cE5hbWUpIHx8IFtdO1xuICAgICAgICBncm91cC5wdXNoKGFzc2V0KTtcbiAgICAgICAgZ3JvdXBlZEFzc2V0cy5zZXQoZ3JvdXBOYW1lLCBncm91cCk7XG4gICAgICB9XG5cbiAgICAgIC8vIFByZXBhcmUgdG8gZ2VuZXJhdGUgaGVhZGVycy5cbiAgICAgIHRyeSB7XG4gICAgICAgIGF3YWl0IGZzLm1rZGlyKG91dHB1dERpciwgeyByZWN1cnNpdmU6IHRydWUgfSk7XG4gICAgICAgIGNvbnN0IHJvdXRlczogc3RyaW5nW10gPSBbXTtcbiAgICAgICAgY29uc3QgaHRtbFJvdXRlczogc3RyaW5nW10gPSBbXTtcbiAgICAgICAgbGV0IHRvdGFsSW5wdXRTaXplID0gMDtcbiAgICAgICAgbGV0IHRvdGFsQ29tcHJlc3NlZFNpemUgPSAwO1xuXG4gICAgICAgIC8vIEdlbmVyYXRlIGhlYWRlciBmaWxlcyBmb3IgZWFjaCBhc3NldCBncm91cC5cbiAgICAgICAgZm9yIChjb25zdCBbZ3JvdXBOYW1lLCBhc3NldHNdIG9mIGdyb3VwZWRBc3NldHMuZW50cmllcygpKSB7XG4gICAgICAgICAgbGV0IGhlYWRlciA9IGAvKlxuICogQmluYXJ5IGFycmF5cyBmb3IgdGhlIFdlYiBVSSAke2dyb3VwTmFtZX0gZmlsZXMuXG4gKi9cblxuI3ByYWdtYSBvbmNlXG4jaW5jbHVkZSA8RVNQQXN5bmNXZWJTZXJ2ZXIuaD5cbiNpbmNsdWRlIDxBcmR1aW5vLmg+XG5cbmA7XG5cbiAgICAgICAgICAvLyBQcm9jZXNzIGVhY2ggYXNzZXQgaW4gdGhlIGdyb3VwLlxuICAgICAgICAgIGZvciAoY29uc3QgYXNzZXQgb2YgYXNzZXRzKSB7XG4gICAgICAgICAgICBjb25zdCBpbnB1dFNpemUgPSBCdWZmZXIuZnJvbShhc3NldC5jb250ZW50KS5sZW5ndGg7XG4gICAgICAgICAgICBjb25zdCBjb21wcmVzc2VkID0gYXdhaXQgY29tcHJlc3NBc3NldChhc3NldC5jb250ZW50LCBhc3NldC5uYW1lLCBhc3NldC5jb250ZW50VHlwZSk7XG4gICAgICAgICAgICBjb21wcmVzc1N0YXRzLnB1c2goe1xuICAgICAgICAgICAgICBmaWxlTmFtZTogYXNzZXQucGF0aCxcbiAgICAgICAgICAgICAgaW5wdXRTaXplLFxuICAgICAgICAgICAgICBjb21wcmVzc2VkU2l6ZTogY29tcHJlc3NlZC5sZW5ndGgsXG4gICAgICAgICAgICAgIGdyb3VwTmFtZSxcbiAgICAgICAgICAgICAgdXNlQ29tcHJlc3Npb246IGNvbXByZXNzZWQudXNlQ29tcHJlc3Npb25cbiAgICAgICAgICAgIH0pO1xuICAgICAgICAgICAgdG90YWxJbnB1dFNpemUgKz0gaW5wdXRTaXplO1xuICAgICAgICAgICAgdG90YWxDb21wcmVzc2VkU2l6ZSArPSBjb21wcmVzc2VkLmxlbmd0aDtcblxuICAgICAgICAgICAgaGVhZGVyICs9IGAvLyAke2Fzc2V0LnBhdGh9XFxuYDtcbiAgICAgICAgICAgIGhlYWRlciArPSBgY29uc3QgdWludDE2X3QgJHtjb21wcmVzc2VkLm5hbWV9X0wgPSAke2NvbXByZXNzZWQubGVuZ3RofTtcXG5gO1xuICAgICAgICAgICAgaGVhZGVyICs9IGBjb25zdCB1aW50OF90ICR7Y29tcHJlc3NlZC5uYW1lfVtdIFBST0dNRU0gPSB7XFxuJHtjb21wcmVzc2VkLmFycmF5fVxcbn07XFxuXFxuYDtcbiAgICAgICAgICAgIGhlYWRlciArPSBgaW5saW5lIHZvaWQgc2VydmUke3Bhc2NhbENhc2UoYXNzZXQubmFtZSl9KEFzeW5jV2ViU2VydmVyUmVxdWVzdCogcmVxdWVzdCkge1xcbmA7XG4gICAgICAgICAgICBoZWFkZXIgKz0gYCAgQXN5bmNXZWJTZXJ2ZXJSZXNwb25zZSAqcmVzcG9uc2UgPSByZXF1ZXN0LT5iZWdpblJlc3BvbnNlX1AoMjAwLCBcIiR7YXNzZXQuY29udGVudFR5cGV9XCIsICR7Y29tcHJlc3NlZC5uYW1lfSwgJHtjb21wcmVzc2VkLm5hbWV9X0wpO1xcbmA7XG4gICAgICAgICAgICBpZiAoY29tcHJlc3NlZC51c2VDb21wcmVzc2lvbikge1xuICAgICAgICAgICAgICBoZWFkZXIgKz0gYCAgcmVzcG9uc2UtPmFkZEhlYWRlcihGKFwiQ29udGVudC1FbmNvZGluZ1wiKSwgXCJnemlwXCIpO1xcbmA7XG4gICAgICAgICAgICB9XG4gICAgICAgICAgICBoZWFkZXIgKz0gYCAgcmVxdWVzdC0+c2VuZChyZXNwb25zZSk7XFxuYDtcbiAgICAgICAgICAgIGhlYWRlciArPSBgfVxcblxcbmA7XG5cbiAgICAgICAgICAgIC8vIEdlbmVyYXRlIHJvdXRlcyBmb3IgdGhpcyBhc3NldC5cbiAgICAgICAgICAgIGNvbnN0IHsgcm91dGVzOiBhc3NldFJvdXRlcywgaHRtbFJvdXRlczogYXNzZXRIdG1sUm91dGVzIH0gPSBnZW5lcmF0ZVJvdXRlc0ZvckFzc2V0KGFzc2V0LCBiYXNlUGF0aCk7XG4gICAgICAgICAgICByb3V0ZXMucHVzaCguLi5hc3NldFJvdXRlcyk7XG4gICAgICAgICAgICBodG1sUm91dGVzLnB1c2goLi4uYXNzZXRIdG1sUm91dGVzKTtcbiAgICAgICAgICB9XG5cbiAgICAgICAgICAvLyBXcml0ZSB0aGUgZ3JvdXAgaGVhZGVyLlxuICAgICAgICAgIGF3YWl0IGZzLndyaXRlRmlsZShyZXNvbHZlKG91dHB1dERpciwgYCR7Z3JvdXBOYW1lfS5oYCksIGhlYWRlcik7XG4gICAgICAgIH1cblxuICAgICAgICAvLyBPdXRwdXQgY29tcHJlc3Npb24gc3RhdHMuXG4gICAgICAgIGNvbnNvbGUubG9nKGNoYWxrLmN5YW4oJ1xcbkdlbmVyYXRpbmcgQysrIGhlYWRlcnMgZm9yIHdlYiBVSSBhc3NldHM6JykpO1xuICAgICAgICBjb25zb2xlLmxvZyhjaGFsay5kaW0oJ1x1MjUwMCcucmVwZWF0KDEwMCkpKTtcbiAgICAgICAgY29tcHJlc3NTdGF0cy5zb3J0KChhLCBiKSA9PiB7XG4gICAgICAgICAgaWYgKGEuZ3JvdXBOYW1lID09PSBiLmdyb3VwTmFtZSkge1xuICAgICAgICAgICAgcmV0dXJuIGIuY29tcHJlc3NlZFNpemUgLSBhLmNvbXByZXNzZWRTaXplO1xuICAgICAgICAgIH1cbiAgICAgICAgICByZXR1cm4gYS5ncm91cE5hbWUubG9jYWxlQ29tcGFyZShiLmdyb3VwTmFtZSk7XG4gICAgICAgIH0pO1xuICAgICAgICBsZXQgY3VycmVudEdyb3VwID0gJyc7XG4gICAgICAgIGZvciAoY29uc3Qgc3RhdCBvZiBjb21wcmVzc1N0YXRzKSB7XG4gICAgICAgICAgaWYgKHN0YXQuZ3JvdXBOYW1lICE9PSBjdXJyZW50R3JvdXApIHtcbiAgICAgICAgICAgIGlmIChjdXJyZW50R3JvdXAgIT09ICcnKSBjb25zb2xlLmxvZygnJyk7XG4gICAgICAgICAgICBjdXJyZW50R3JvdXAgPSBzdGF0Lmdyb3VwTmFtZTtcbiAgICAgICAgICB9XG4gICAgICAgICAgY29uc3QgcmF0aW8gPSBzdGF0LmNvbXByZXNzZWRTaXplIC8gc3RhdC5pbnB1dFNpemU7XG4gICAgICAgICAgY29uc3QgY29tcHJlc3Npb25SZXN1bHQgPSBzdGF0LnVzZUNvbXByZXNzaW9uID8gY2hhbGsuZGltKCdnemlwJykgOiBjaGFsay55ZWxsb3coJ3VuY29tcHJlc3NlZCcpO1xuICAgICAgICAgIGNvbnNvbGUubG9nKFxuICAgICAgICAgICAgY2hhbGsuZGltKHBhZEVuZChzdGF0LmZpbGVOYW1lLCA3MCkpICtcbiAgICAgICAgICAgIGNoYWxrLmJsdWUocGFkRW5kKGZvcm1hdFNpemUoc3RhdC5pbnB1dFNpemUpLCAxMCkpICtcbiAgICAgICAgICAgIGNvbXByZXNzaW9uUmVzdWx0ICsgJzogJyArXG4gICAgICAgICAgICBjaGFsay5ncmVlbihmb3JtYXRTaXplKHN0YXQuY29tcHJlc3NlZFNpemUpKSArXG4gICAgICAgICAgICBjaGFsay5kaW0oYCAoJHsocmF0aW8gKiAxMDApLnRvRml4ZWQoMSl9JSlgKVxuICAgICAgICAgICk7XG4gICAgICAgIH1cbiAgICAgICAgY29uc29sZS5sb2coY2hhbGsuZGltKCdcXG4nICsgJ1x1MjUwMCcucmVwZWF0KDEwMCkpKTtcbiAgICAgICAgY29uc29sZS5sb2coXG4gICAgICAgICAgY2hhbGsuY3lhbignVG90YWwgY29tcHJlc3NlZCBzaXplOiAnKSArXG4gICAgICAgICAgY2hhbGsuZ3JlZW4oZm9ybWF0U2l6ZSh0b3RhbENvbXByZXNzZWRTaXplKSkgK1xuICAgICAgICAgIGNoYWxrLmRpbShgICgkeygodG90YWxDb21wcmVzc2VkU2l6ZSAvIHRvdGFsSW5wdXRTaXplKSAqIDEwMCkudG9GaXhlZCgxKX0lIG9mICR7Zm9ybWF0U2l6ZSh0b3RhbElucHV0U2l6ZSl9KWApXG4gICAgICAgICk7XG4gICAgICAgIGNvbnNvbGUubG9nKGNoYWxrLmdyZWVuKGBHZW5lcmF0ZWQgQysrIGhlYWRlcnMgaW4gJHtvdXRwdXREaXJ9XFxuYCkpO1xuXG4gICAgICAgIC8vIEdlbmVyYXRlIHRoZSByb3V0ZXMgaGVhZGVyLlxuICAgICAgICBjb25zdCBncm91cFNpemVDb21tZW50cyA9IEFycmF5LmZyb20oZ3JvdXBlZEFzc2V0cy5lbnRyaWVzKCkpLm1hcCgoW2dyb3VwTmFtZSwgYXNzZXRzXSkgPT4ge1xuICAgICAgICAgIGNvbnN0IGdyb3VwU3RhdHMgPSBjb21wcmVzc1N0YXRzLmZpbHRlcihzdGF0ID0+IHN0YXQuZ3JvdXBOYW1lID09PSBncm91cE5hbWUpO1xuICAgICAgICAgIGNvbnN0IHRvdGFsR3JvdXBCeXRlcyA9IGdyb3VwU3RhdHMucmVkdWNlKChzdW0sIHN0YXQpID0+IHN1bSArIHN0YXQuY29tcHJlc3NlZFNpemUsIDApO1xuICAgICAgICAgIHJldHVybiBgICogJHtncm91cE5hbWV9OiAke3RvdGFsR3JvdXBCeXRlcy50b0xvY2FsZVN0cmluZygpfSBieXRlc2A7XG4gICAgICAgIH0pO1xuICAgICAgICBjb25zdCByb3V0ZXNIZWFkZXIgPSBgLypcbiAqIFdlYiBVSSBSb3V0ZXNcbiAqXG4gKiBDb21wcmVzc2VkIFNpemUgU3VtbWFyeTpcbiR7Z3JvdXBTaXplQ29tbWVudHMuam9pbignXFxuJyl9XG4gKiBUb3RhbDogJHt0b3RhbENvbXByZXNzZWRTaXplLnRvTG9jYWxlU3RyaW5nKCl9IGJ5dGVzXG4gKi9cblxuI3ByYWdtYSBvbmNlXG5cbiNpbmNsdWRlIDxFU1BBc3luY1dlYlNlcnZlci5oPlxuJHtBcnJheS5mcm9tKGdyb3VwZWRBc3NldHMua2V5cygpKS5tYXAoZ3JvdXAgPT4gYCNpbmNsdWRlIFwiJHtncm91cH0uaFwiYCkuam9pbignXFxuJyl9XG5cbmlubGluZSB2b2lkIHNldHVwUm91dGVzKEFzeW5jV2ViU2VydmVyKiBzZXJ2ZXIpIHtcbiR7cm91dGVzLmpvaW4oJ1xcbicpfVxuICAgIC8vIEhUTUwgcm91dGVzXG4ke2h0bWxSb3V0ZXMuam9pbignXFxuJyl9XG59XG5gO1xuICAgICAgICBhd2FpdCBmcy53cml0ZUZpbGUocmVzb2x2ZShvdXRwdXREaXIsIGAke291dFByZWZpeH1yb3V0ZXMuaGApLCByb3V0ZXNIZWFkZXIpO1xuICAgICAgfSBjYXRjaCAoZXJyb3IpIHtcbiAgICAgICAgY29uc29sZS5lcnJvcihjaGFsay5yZWQoYEVycm9yIHdyaXRpbmcgb3V0cHV0IGZpbGVzIHRvICR7b3V0cHV0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
