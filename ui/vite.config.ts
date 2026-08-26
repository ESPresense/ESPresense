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
        // Prefer one shared JS payload for flash size. SvelteKit still emits tiny
        // entry/node stubs; this collapses app code into a single chunk group.
        rolldownOptions: {
            output: {
                codeSplitting: {
                    groups: [
                        {
                            name: 'index',
                            test: (id) => !id.includes('node_modules/.vite'),
                            includeDependenciesRecursively: true,
                        }
                    ]
                }
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
