import { defineConfig } from '@playwright/test';
import { fileURLToPath } from 'url';
import { dirname } from 'path';

const __dirname = dirname(fileURLToPath(import.meta.url));

export default defineConfig({
  testDir: './tests',
  webServer: {
    command: 'npx vite build && python3 -m http.server 5173 --directory build',
    port: 5173,
    cwd: __dirname,
    timeout: 120 * 1000,
    reuseExistingServer: !process.env.CI,
  },
  use: { baseURL: 'http://localhost:5173' },
});
