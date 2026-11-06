import { createHash } from 'crypto';
import { readdirSync, readFileSync, statSync } from 'fs';
import { join, relative } from 'path';

// Everything that can change the built UI bundle. Specs and test setup are left
// out so a test-only change doesn't churn the generated firmware headers.
const INPUTS = ['src', 'static', 'package-lock.json', 'vite.config.ts', 'postcss.config.js', 'tsconfig.json'];
const SKIP = /(\.spec\.[jt]s|\.test\.[jt]s|setupTests\.ts)$/;

function walk(root: string, path: string, out: string[]): void {
  let st;
  try {
    st = statSync(path);
  } catch {
    return;
  }
  if (st.isDirectory()) {
    for (const name of readdirSync(path)) {
      if (!name.startsWith('.')) walk(root, join(path, name), out);
    }
  } else if (!SKIP.test(path)) {
    out.push(relative(root, path).split('\\').join('/'));
  }
}

/**
 * A short content hash of the UI sources, used as SvelteKit's `version.name`.
 *
 * SvelteKit bakes `version.name` into the bundle and writes it to `app/version.json`,
 * which the firmware serves (ui/plugins/cpp.ts). `updated.check()` compares the two, so
 * after an OTA that changed the UI an open page sees a new version and does a full
 * reload instead of a broken client-side navigation. It has to be deterministic
 * (SvelteKit's default is Date.now()) so the committed `main/ui_*.h` headers only change
 * when the UI does. Line endings are normalised so a Windows checkout hashes the same.
 */
export function uiSourceVersion(root: string): string {
  const files: string[] = [];
  for (const input of INPUTS) walk(root, join(root, input), files);
  files.sort();

  const hash = createHash('sha256');
  for (const file of files) {
    const content = readFileSync(join(root, file)).toString('latin1').replace(/\r\n/g, '\n');
    hash.update(file).update('\0').update(content, 'latin1').update('\0');
  }
  return hash.digest('hex').slice(0, 12);
}
