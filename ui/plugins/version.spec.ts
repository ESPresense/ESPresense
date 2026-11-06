import { describe, it, expect, beforeEach, afterEach } from 'vitest';
import { mkdtempSync, mkdirSync, rmSync, writeFileSync } from 'fs';
import { tmpdir } from 'os';
import { join } from 'path';
import { uiSourceVersion } from './version';

describe('uiSourceVersion', () => {
    let root: string;
    beforeEach(() => {
        root = mkdtempSync(join(tmpdir(), 'ui-version-'));
        mkdirSync(join(root, 'src/routes'), { recursive: true });
        writeFileSync(join(root, 'src/routes/+page.svelte'), '<h1>hi</h1>\n');
        writeFileSync(join(root, 'package-lock.json'), '{}\n');
    });
    afterEach(() => rmSync(root, { recursive: true, force: true }));

    it('is a stable 12-hex-digit hash', () => {
        const v = uiSourceVersion(root);
        expect(v).toMatch(/^[0-9a-f]{12}$/);
        expect(uiSourceVersion(root)).toBe(v);
    });

    it('changes when the UI sources change', () => {
        const before = uiSourceVersion(root);
        writeFileSync(join(root, 'src/routes/+page.svelte'), '<h1>hello</h1>\n');
        expect(uiSourceVersion(root)).not.toBe(before);
    });

    it('changes when a dependency changes', () => {
        const before = uiSourceVersion(root);
        writeFileSync(join(root, 'package-lock.json'), '{"x":1}\n');
        expect(uiSourceVersion(root)).not.toBe(before);
    });

    it('ignores specs and line endings', () => {
        const before = uiSourceVersion(root);
        writeFileSync(join(root, 'src/routes/page.spec.ts'), 'test');
        writeFileSync(join(root, 'src/routes/+page.svelte'), '<h1>hi</h1>\r\n');
        expect(uiSourceVersion(root)).toBe(before);
    });
});

