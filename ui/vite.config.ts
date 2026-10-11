import adapter from '@sveltejs/adapter-static';
import { vitePreprocess } from '@sveltejs/vite-plugin-svelte';
import { sveltekit } from '@sveltejs/kit/vite';
import { defineConfig } from 'vite';
import { cppPlugin } from './plugins/cpp';
import strip from '@rollup/plugin-strip';
import tailwindcss from '@tailwindcss/vite';

// `npm run dev` proxies the API to a live node: ESPRESENSE_DEVICE=192.168.129.120 npm run dev
const device = process.env.ESPRESENSE_DEVICE ?? '192.168.129.97';

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
                    '/templates',
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
            '/json': `http://${device}/`,
            '/wifi': `http://${device}/`,
            '/restart': `http://${device}/`,
            '/ws': {
                target: `ws://${device}/`,
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
