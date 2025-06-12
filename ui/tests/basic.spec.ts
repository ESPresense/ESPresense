import { test, expect } from '@playwright/test';

test('network page loads', async ({ page }) => {
  const response = await page.goto('/network');
  expect(response?.ok()).toBeTruthy();
  await expect(page.getByRole('heading', { name: /Room Configuration/i })).toBeVisible();
});

test('devices page loads', async ({ page }) => {
  const response = await page.goto('/devices');
  expect(response?.ok()).toBeTruthy();
  await expect(page.getByRole('heading', { name: /Devices/i })).toBeVisible();
});

test('fingerprints page loads', async ({ page }) => {
  const response = await page.goto('/fingerprints');
  expect(response?.ok()).toBeTruthy();
  await expect(page.getByRole('heading', { name: /Fingerprints/i })).toBeVisible();
});

test('settings page loads', async ({ page }) => {
  const response = await page.goto('/settings');
  expect(response?.ok()).toBeTruthy();
  await expect(page.getByRole('heading', { name: /Scanning/i })).toBeVisible();
});
