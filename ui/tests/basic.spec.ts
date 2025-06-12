import { test, expect } from '@playwright/test';

test('network page loads', async ({ page }) => {
  const response = await page.goto('/network.html');
  expect(response?.ok()).toBeTruthy();
  await expect(page).toHaveTitle(/ESPresense/i);
});

test('devices page loads', async ({ page }) => {
  const response = await page.goto('/devices.html');
  expect(response?.ok()).toBeTruthy();
  await expect(page).toHaveTitle(/ESPresense/i);
});

test('fingerprints page loads', async ({ page }) => {
  const response = await page.goto('/fingerprints.html');
  expect(response?.ok()).toBeTruthy();
  await expect(page).toHaveTitle(/ESPresense/i);
});

test('settings page loads', async ({ page }) => {
  const response = await page.goto('/settings.html');
  expect(response?.ok()).toBeTruthy();
  await expect(page).toHaveTitle(/ESPresense/i);
});
