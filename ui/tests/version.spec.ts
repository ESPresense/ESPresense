import { test, expect } from '@playwright/test';
import { readFileSync } from 'fs';

// SvelteKit's updated.check() fetches /app/version.json on tab focus (#2637). The preview
// server always has it; the firmware only serves what ui/plugins/cpp.ts registers.
test('the firmware serves app/version.json', () => {
    const routes = readFileSync(new URL('../../main/ui_routes.h', import.meta.url), 'utf8');
    expect(routes).toContain('registerGet(server, "/app/version.json"');
});
