import { test, expect } from '@playwright/test';

test('network page loads', async ({ page }) => {
  const response = await page.goto('/network');
  expect(response?.ok()).toBeTruthy();
  await expect(page).toHaveTitle(/ESPresense/i);
});

test('devices page loads', async ({ page }) => {
  const response = await page.goto('/devices');
  expect(response?.ok()).toBeTruthy();
  await expect(page).toHaveTitle(/ESPresense/i);
});

test('fingerprints page loads', async ({ page }) => {
  const response = await page.goto('/fingerprints');
  expect(response?.ok()).toBeTruthy();
  await expect(page).toHaveTitle(/ESPresense/i);
});

test('settings page loads', async ({ page }) => {
  const response = await page.goto('/settings');
  expect(response?.ok()).toBeTruthy();
  await expect(page).toHaveTitle(/ESPresense/i);
});
