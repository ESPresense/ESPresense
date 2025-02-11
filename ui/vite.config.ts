import { sveltekit } from '@sveltejs/kit/vite';
import { defineConfig } from 'vite';
import { cppPlugin } from './plugins/cpp';
import strip from '@rollup/plugin-strip';

export default defineConfig({
    plugins: [
        sveltekit(),
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
            '/json': 'http://192.168.129.176',
            '/wifi': 'http://192.168.129.176',
            '/restart': 'http://192.168.129.176',
            '/ws': {
                target: 'ws://192.168.129.176',
                ws: true,
            }
        }
    }
});
