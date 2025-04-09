import { sveltekit } from '@sveltejs/kit/vite';
import { defineConfig } from 'vite';
import { cppPlugin } from './plugins/cpp';
import strip from '@rollup/plugin-strip';
import tailwindcss from '@tailwindcss/vite';

export default defineConfig({
    plugins: [
        tailwindcss(),
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
            '/json': 'http://192.168.129.145/',
            '/wifi': 'http://192.168.129.145/',
            '/restart': 'http://192.168.129.145/',
            '/ws': {
                target: 'ws://192.168.129.145/',
                ws: true,
            }
        }
    }
});
