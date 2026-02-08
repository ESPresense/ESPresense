import { describe, it, expect, beforeEach, afterEach, vi } from 'vitest';
import { render, screen, fireEvent, cleanup } from '@testing-library/svelte';

describe('DarkModeToggle', () => {
  const key = 'theme';
  let prefersDark = false;

  const mockMatchMedia = () =>
    vi.fn().mockImplementation((query: string) => ({
      matches: prefersDark,
      media: query,
      onchange: null,
      addEventListener: vi.fn(),
      removeEventListener: vi.fn(),
      addListener: vi.fn(),
      removeListener: vi.fn(),
      dispatchEvent: vi.fn()
    }));

  const localStorageMock = (() => {
    let store: Record<string, string> = {};
    return {
      getItem: vi.fn((key: string) => store[key] || null),
      setItem: vi.fn((key: string, value: string) => {
        store[key] = value.toString();
      }),
      clear: vi.fn(() => {
        store = {};
      }),
      removeItem: vi.fn((key: string) => {
        delete store[key];
      }),
    };
  })();

  Object.defineProperty(window, 'localStorage', {
    value: localStorageMock,
  });

  beforeEach(async () => {
    // Reset DOM class and storage before each test
    // vi.resetModules(); // Causes Svelte component to crash
    const { themeMode } = await import('$lib/stores');
    themeMode.set('system');

    document.documentElement.classList.remove('dark');
    localStorage.clear();
    prefersDark = false;
    window.matchMedia = mockMatchMedia();
  });

  afterEach(() => {
    document.documentElement.classList.remove('dark');
    localStorage.clear();
    cleanup();
  });

  it('sets dark theme on click from light', async () => {
    localStorage.setItem(key, 'light');
    const { default: DarkModeToggle } = await import('./DarkModeToggle.svelte');
    render(DarkModeToggle as any);

    const btn = screen.getByRole('button', { name: /dark theme/i });
    expect(document.documentElement).not.toHaveClass('dark');

    await fireEvent.click(btn);
    expect(document.documentElement).toHaveClass('dark');
    expect(localStorage.getItem(key)).toBe('dark');
  });

  it('sets light theme on click from dark', async () => {
    localStorage.setItem(key, 'dark');
    const { themeMode } = await import('$lib/stores');
    themeMode.set('dark');
    const { default: DarkModeToggle } = await import('./DarkModeToggle.svelte');
    render(DarkModeToggle as any);
    const btn = screen.getByRole('button', { name: /light theme/i });

    await fireEvent.click(btn);
    expect(document.documentElement).not.toHaveClass('dark');
    expect(localStorage.getItem(key)).toBe('light');
  });

  it('uses system preference when selecting system theme', async () => {
    prefersDark = true;
    window.matchMedia = mockMatchMedia();
    localStorage.setItem(key, 'light');
    const { themeMode } = await import('$lib/stores');
    themeMode.set('light');
    const { default: DarkModeToggle } = await import('./DarkModeToggle.svelte');
    render(DarkModeToggle as any);

    const btn = screen.getByRole('button', { name: /system theme/i });
    await fireEvent.click(btn);

    expect(document.documentElement).toHaveClass('dark');
    expect(localStorage.getItem(key)).toBe('system');
  });
});
