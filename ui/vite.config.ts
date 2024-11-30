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
        cppPlugin()
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
            '/json': {
                target: 'http://192.168.128.165',
                changeOrigin: true,
            },
            '/extras': {
                target: 'http://192.168.128.165',
                changeOrigin: true,
            },
            '/ws': {
                target: 'ws://192.168.128.165',
                ws: true,
            }
        }
    }
});
