import { sveltekit } from '@sveltejs/kit/vite';
import { defineConfig } from 'vite';
import { cppPlugin } from './plugins/cpp';
import strip from '@rollup/plugin-strip';
import tailwindcss from '@tailwindcss/vite';

export default defineConfig({
    plugins: [
        sveltekit(),
        tailwindcss(),
        strip({
            include: '**/*.(js|ts|svelte)',
            functions: ['console.*', 'assert.*'],
        }),
        cppPlugin({ basePath: '', outPrefix: 'ui_' })
    ],
    build: {
        sourcemap: false,
        rollupOptions: {
            output: {
                manualChunks: () => 'index'
            }
        }
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
