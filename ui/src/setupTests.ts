// Vitest setup for DOM matchers and Svelte components
import '@testing-library/jest-dom/vitest';

// Provide a basic matchMedia mock for components that read it
if (typeof window !== 'undefined') {
  Object.defineProperty(window, 'matchMedia', {
    value: (query: string) => ({
      matches: false,
      media: query,
      onchange: null,
      addListener: () => {},
      removeListener: () => {},
      addEventListener: () => {},
      removeEventListener: () => {},
      dispatchEvent: () => false,
    }),
    writable: true,
  });
}
