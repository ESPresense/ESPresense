import { describe, it, expect } from 'vitest';
import { render, screen, fireEvent } from '@testing-library/svelte';
import HamburgerButton from './HamburgerButton.svelte';

describe('HamburgerButton', () => {
  it('toggles aria-expanded on click', async () => {
    render(HamburgerButton);
    const btn = screen.getByRole('button', { name: /open main menu/i });

    expect(btn).toHaveAttribute('aria-expanded', 'false');
    await fireEvent.click(btn);
    expect(btn).toHaveAttribute('aria-expanded', 'true');
    await fireEvent.click(btn);
    expect(btn).toHaveAttribute('aria-expanded', 'false');
  });
});

