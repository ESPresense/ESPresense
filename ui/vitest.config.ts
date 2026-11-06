import { sveltekit } from '@sveltejs/kit/vite';
import { defineConfig } from 'vitest/config';

export default defineConfig({
  plugins: [sveltekit()],
  resolve: {
    conditions: ['browser'],
  },
  test: {
    environment: 'jsdom',
    setupFiles: ['src/setupTests.ts'],
    include: ['src/**/*.spec.{ts,js}'],
    css: true,
    pool: 'threads',
    // No spec relies on a fresh module graph; sharing one is faster.
    isolate: false,
  },
});
