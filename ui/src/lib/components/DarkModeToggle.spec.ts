import { describe, it, expect, beforeEach, afterEach } from 'vitest';
import { render, screen, fireEvent, cleanup, waitFor } from '@testing-library/svelte';

describe('DarkModeToggle', () => {
  const key = 'darkMode';

  beforeEach(() => {
    // Reset DOM class and storage before each test
    document.documentElement.classList.remove('dark');
    localStorage.clear();
  });

  afterEach(() => {
    document.documentElement.classList.remove('dark');
    localStorage.clear();
    cleanup();
  });

  it('toggles dark class on click from light to dark', async () => {
    localStorage.setItem(key, 'false');
    const { default: DarkModeToggle } = await import('./DarkModeToggle.svelte');
    render(DarkModeToggle as any);

    const btn = screen.getByRole('button', { name: /toggle dark mode/i });
    expect(document.documentElement).not.toHaveClass('dark');

    await fireEvent.click(btn);
    expect(document.documentElement).toHaveClass('dark');
    expect(localStorage.getItem(key)).toBe('true');
  });

  it('toggles dark class off when already dark', async () => {
    localStorage.setItem(key, 'true');
    // Ensure store is initialized and reflects dark=true
    const { darkMode } = await import('$lib/stores');
    darkMode.set(true);
    const { default: DarkModeToggle } = await import('./DarkModeToggle.svelte');
    render(DarkModeToggle as any);
    const btn = screen.getByRole('button', { name: /toggle dark mode/i });

    await fireEvent.click(btn);
    expect(document.documentElement).not.toHaveClass('dark');
    expect(localStorage.getItem(key)).toBe('false');
  });
});
