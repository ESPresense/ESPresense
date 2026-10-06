import adapter from '@sveltejs/adapter-static';
import { vitePreprocess } from '@sveltejs/vite-plugin-svelte';
import { sveltekit } from '@sveltejs/kit/vite';
import { defineConfig } from 'vite';
import { cppPlugin } from './plugins/cpp';
import strip from '@rollup/plugin-strip';
import tailwindcss from '@tailwindcss/vite';

export default defineConfig({
    plugins: [
        sveltekit({
            preprocess: vitePreprocess(),
            adapter: adapter(),
            prerender: {
                crawl: false,
                entries: [
                    '/',
                    '/settings',
                    '/hardware',
                    '/devices',
                    '/fingerprints',
                    '/network'
                ],
                handleHttpError: 'warn'
            },
            appDir: 'app',
            // Collapse to one shared JS payload for flash size. SK3's built-in
            // 'single' replaces the hand-rolled rolldown codeSplitting group,
            // which broke SK3's 'split' bundle logic ("Could not find the
            // client runtime chunk").
            output: { bundleStrategy: 'single' },
            paths: { base: '' },
            version: { name: '', pollInterval: 0 }
        }),
        tailwindcss(),
        strip({
            include: '**/*.(js|ts|svelte)',
            functions: ['console.*', 'assert.*'],
        }),
        cppPlugin({ basePath: '', outPrefix: 'ui_' })
    ],
    build: {
        sourcemap: false
    },
    server: {
        proxy: {
            '/json': 'http://192.168.129.97/',
            '/wifi': 'http://192.168.129.97/',
            '/restart': 'http://192.168.129.97/',
            '/ws': {
                target: 'ws://192.168.129.97/',
                ws: true,
            }
        }
    },
    preview: {
        port: 4173,
        // Don't proxy API requests in preview mode - let tests handle mocking
        proxy: {}
    }
});
