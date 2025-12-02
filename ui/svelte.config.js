import adapter from '@sveltejs/adapter-static';
import { vitePreprocess } from '@sveltejs/vite-plugin-svelte';

/** @type {import('@sveltejs/kit').Config} */
const config = {
    preprocess: vitePreprocess(),

    kit: {
        adapter: adapter(),
        prerender: {
            crawl: false,
            entries: ['/', '/settings', '/devices', '/fingerprints', '/network'],
            handleHttpError: 'warn',
        },
        appDir: 'app',
        paths: {
            base: ''
        },
        version: {
            name: '',
            pollInterval: 0
        }
    }
};

export default config;
