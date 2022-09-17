import { defineConfig } from 'vite'
import { svelte } from '@sveltejs/vite-plugin-svelte'

// https://vitejs.dev/config/
export default defineConfig({
  plugins: [svelte()],
  server: {
    proxy: {
      '^/json': {
        target: 'http://espresense-living-room.localdomain/',
        changeOrigin: true,
        rewrite: (path) => path
      },
      '^/ws': {
        target: 'ws://espresense-living-room.localdomain/',
        changeOrigin: true,
        rewrite: (path) => path
      },
    }
  }
})
