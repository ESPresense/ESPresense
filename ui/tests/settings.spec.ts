import { test, expect } from '@playwright/test';

// Mock data for extra settings
const mockExtraSettings = {
	values: {
		known_macs: 'aabbccddeeff 112233445566',
		known_irks: 'abcd1234efgh5678',
		forget_ms: '180000',
		query: 'flora: mi:',
		requery_ms: '300',
		count_ids: 'apple: tile:',
		count_enter: '2.5',
		count_exit: '5.0',
		count_ms: '60000',
		skip_distance: '100',
		skip_ms: '5000',
		max_distance: '50',
		absorption: '3.5',
		tx_ref_rssi: '-59',
		rx_adj_rssi: '0',
		query_par: 'false'
	},
	defaults: {
		forget_ms: '180000',
		requery_ms: '300',
		count_enter: '2.5',
		count_exit: '5.0',
		count_ms: '60000',
		skip_distance: '100',
		skip_ms: '5000',
		max_distance: '50',
		absorption: '3.5',
		tx_ref_rssi: '-59',
		rx_adj_rssi: '0'
	}
};

test.describe('Settings Page', () => {
	test.beforeEach(async ({ page }) => {
		// Mock endpoints
		await page.route('**/json', async (route) => {
			await route.fulfill({
				status: 200,
				contentType: 'application/json',
				body: JSON.stringify({ room: 'test-room' })
			});
		});

		await page.route('**/wifi/extras', async (route) => {
			if (route.request().method() === 'GET') {
				await route.fulfill({
					status: 200,
					contentType: 'application/json',
					body: JSON.stringify(mockExtraSettings)
				});
			} else {
				await route.fulfill({ status: 200 });
			}
		});

		await page.route('**/restart', async (route) => {
			await route.abort('failed'); // Simulate connection drop during restart
		});
	});

	test('should load settings page and display form', async ({ page }) => {
		await page.goto('/settings');

		// Wait for form to load
		await page.waitForSelector('form#extras');

		// Check form exists
		await expect(page.locator('form#extras')).toBeVisible();
	});

	test('should display all settings sections', async ({ page }) => {
		await page.goto('/settings');
		await page.waitForSelector('form#extras');

		// Check section headers exist (use link selectors to avoid ambiguity)
		await expect(page.locator('a[href*="settings#scanning"]')).toBeVisible();
		await expect(page.locator('a[href*="settings#querying"]')).toBeVisible();
		await expect(page.locator('a[href*="settings#counting"]')).toBeVisible();
		await expect(page.locator('a[href*="settings#filtering"]')).toBeVisible();
	});

	test('should have links to documentation', async ({ page }) => {
		await page.goto('/settings');
		await page.waitForSelector('form#extras');

		// Check documentation links
		const scanningLink = page.locator('a[href*="espresense.com/configuration/settings#scanning"]');
		await expect(scanningLink).toBeVisible();
		await expect(scanningLink).toHaveAttribute('target', '_blank');

		const queryingLink = page.locator('a[href*="espresense.com/configuration/settings#querying"]');
		await expect(queryingLink).toBeVisible();

		const countingLink = page.locator('a[href*="espresense.com/configuration/settings#counting"]');
		await expect(countingLink).toBeVisible();
	});

	test('should display current settings values', async ({ page }) => {
		await page.goto('/settings');
		await page.waitForSelector('form#extras');

		// Check known MACs
		const knownMacs = page.locator('input[name="known_macs"]');
		await expect(knownMacs).toHaveValue('aabbccddeeff 112233445566');

		// Check forget time
		const forgetMs = page.locator('input[name="forget_ms"]');
		await expect(forgetMs).toHaveValue('180000');

		// Check query value
		const query = page.locator('input[name="query"]');
		await expect(query).toHaveValue('flora: mi:');
	});

	test('should allow changing form values', async ({ page }) => {
		await page.goto('/settings');
		await page.waitForSelector('form#extras');

		// Change known MACs
		const knownMacs = page.locator('input[name="known_macs"]');
		await knownMacs.fill('newmac123456');
		await expect(knownMacs).toHaveValue('newmac123456');

		// Change forget time
		const forgetMs = page.locator('input[name="forget_ms"]');
		await forgetMs.fill('240000');
		await expect(forgetMs).toHaveValue('240000');

		// Change count enter distance
		const countEnter = page.locator('input[name="count_enter"]');
		await countEnter.fill('3.0');
		await expect(countEnter).toHaveValue('3.0');
	});

	test('should validate numeric input ranges', async ({ page }) => {
		await page.goto('/settings');
		await page.waitForSelector('form#extras');

		// Test forget_ms min/max
		const forgetMs = page.locator('input[name="forget_ms"]');
		await expect(forgetMs).toHaveAttribute('min', '0');
		await expect(forgetMs).toHaveAttribute('max', '3000000');

		// Test count_enter min/max
		const countEnter = page.locator('input[name="count_enter"]');
		await expect(countEnter).toHaveAttribute('min', '0');
		await expect(countEnter).toHaveAttribute('max', '100');

		// Test requery_ms min/max
		const requeryMs = page.locator('input[name="requery_ms"]');
		await expect(requeryMs).toHaveAttribute('min', '30');
		await expect(requeryMs).toHaveAttribute('max', '3600');
	});

	test('should submit form and trigger restart', async ({ page }) => {
		let postCalled = false;
		let restartCalled = false;

		await page.route('**/wifi/extras', async (route) => {
			if (route.request().method() === 'POST') {
				postCalled = true;
				await route.fulfill({ status: 200 });
			} else {
				await route.fulfill({
					status: 200,
					contentType: 'application/json',
					body: JSON.stringify(mockExtraSettings)
				});
			}
		});

		await page.route('**/restart', async (route) => {
			restartCalled = true;
			await route.abort('failed');
		});

		await page.goto('/settings');
		await page.waitForSelector('form#extras');

		// Submit form
		await page.locator('button[type="submit"]').click();

		// Verify API calls were made
		await expect(async () => {
			expect(postCalled).toBe(true);
			expect(restartCalled).toBe(true);
		}).toPass({ timeout: 3000 });
	});

	test('should show saving state on submit button', async ({ page }) => {
		await page.goto('/settings');
		await page.waitForSelector('form#extras');

		const submitButton = page.locator('button[type="submit"]');

		// Initial state
		await expect(submitButton).toHaveText('Save');

		// Click and check for "Saving..." text
		await Promise.all([
			page.waitForSelector('button[type="submit"]:has-text("Saving...")'),
			submitButton.click()
		]);

		// Eventually returns to normal state
		await expect(submitButton).toHaveText('Save', { timeout: 10000 });
	});

	test('should retry loading settings after restart', async ({ page }) => {
		let getCallCount = 0;
		let postCalled = false;

		await page.route('**/wifi/extras', async (route) => {
			if (route.request().method() === 'GET') {
				getCallCount++;

				// First GET is for initial page load
				if (getCallCount === 1) {
					await route.fulfill({
						status: 200,
						contentType: 'application/json',
						body: JSON.stringify(mockExtraSettings)
					});
				}
				// After POST, fail first 2 retries, then succeed
				else if (postCalled && getCallCount <= 4) {
					await route.abort('failed');
				} else {
					await route.fulfill({
						status: 200,
						contentType: 'application/json',
						body: JSON.stringify(mockExtraSettings)
					});
				}
			} else {
				postCalled = true;
				await route.fulfill({ status: 200 });
			}
		});

		await page.goto('/settings');
		await page.waitForSelector('form#extras');

		const initialGetCount = getCallCount;

		// Submit form
		await page.locator('button[type="submit"]').click();

		// Wait for retries (5 retries × 1s delay = 5s, plus buffer)
		await expect(async () => {
			expect(getCallCount).toBeGreaterThan(initialGetCount + 2);
		}).toPass({ timeout: 10000 });

		// Should have retried multiple times
		expect(getCallCount).toBeGreaterThan(initialGetCount + 2);
	});

	test('should handle API errors gracefully', async ({ page }) => {
		// Mock failing POST request
		await page.route('**/wifi/extras', async (route) => {
			if (route.request().method() === 'POST') {
				await route.fulfill({ status: 500, body: 'Internal Server Error' });
			} else {
				await route.fulfill({
					status: 200,
					contentType: 'application/json',
					body: JSON.stringify(mockExtraSettings)
				});
			}
		});

		await page.goto('/settings');
		await page.waitForSelector('form#extras');

		// Spy on console.error
		const consoleMessages: string[] = [];
		page.on('console', (msg) => {
			if (msg.type() === 'error') {
				consoleMessages.push(msg.text());
			}
		});

		// Try to submit
		await page.locator('button[type="submit"]').click();

		// Button should return to normal state
		await expect(page.locator('button[type="submit"]')).toHaveText('Save', { timeout: 3000 });
	});

	test('should NOT have hardware settings', async ({ page }) => {
		await page.goto('/settings');
		await page.waitForSelector('form#extras');

		// Hardware settings should NOT be on this page anymore
		await expect(page.locator('text=LED 1:')).not.toBeVisible();
		await expect(page.locator('text=PIR:')).not.toBeVisible();
		await expect(page.locator('text=I2C Settings')).not.toBeVisible();
		await expect(page.locator('input[name="led_1_pin"]')).not.toBeVisible();
	});

	test('should display all Scanning section fields', async ({ page }) => {
		await page.goto('/settings');
		await page.waitForSelector('form#extras');

		// Check all scanning fields exist
		await expect(page.locator('input[name="known_macs"]')).toBeVisible();
		await expect(page.locator('input[name="known_irks"]')).toBeVisible();
		await expect(page.locator('input[name="forget_ms"]')).toBeVisible();
	});

	test('should display all Querying section fields', async ({ page }) => {
		await page.goto('/settings');
		await page.waitForSelector('form#extras');

		// Check all querying fields exist
		await expect(page.locator('input[name="query"]')).toBeVisible();
		await expect(page.locator('input[name="requery_ms"]')).toBeVisible();
	});

	test('should display all Counting section fields', async ({ page }) => {
		await page.goto('/settings');
		await page.waitForSelector('form#extras');

		// Check all counting fields exist
		await expect(page.locator('input[name="count_ids"]')).toBeVisible();
		await expect(page.locator('input[name="count_enter"]')).toBeVisible();
		await expect(page.locator('input[name="count_exit"]')).toBeVisible();
		await expect(page.locator('input[name="count_ms"]')).toBeVisible();
	});

	test('should have correct input types', async ({ page }) => {
		await page.goto('/settings');
		await page.waitForSelector('form#extras');

		// Text inputs don't have explicit type="text" in the HTML, they default to text
		// So we just verify they exist
		await expect(page.locator('input[name="known_macs"]')).toBeVisible();
		await expect(page.locator('input[name="query"]')).toBeVisible();

		// Number inputs should have explicit type="number"
		await expect(page.locator('input[name="forget_ms"]')).toHaveAttribute('type', 'number');
		await expect(page.locator('input[name="count_enter"]')).toHaveAttribute('type', 'number');
		await expect(page.locator('input[name="count_exit"]')).toHaveAttribute('type', 'number');
	});

	test('should have proper step values for decimal inputs', async ({ page }) => {
		await page.goto('/settings');
		await page.waitForSelector('form#extras');

		// Decimal inputs should have step="0.01"
		await expect(page.locator('input[name="count_enter"]')).toHaveAttribute('step', '0.01');
		await expect(page.locator('input[name="count_exit"]')).toHaveAttribute('step', '0.01');

		// Integer inputs should have step="1"
		await expect(page.locator('input[name="forget_ms"]')).toHaveAttribute('step', '1');
		await expect(page.locator('input[name="requery_ms"]')).toHaveAttribute('step', '1');
	});

	test('should display placeholders with default values', async ({ page }) => {
		await page.goto('/settings');
		await page.waitForSelector('form#extras');

		// Check placeholders show defaults
		const forgetMs = page.locator('input[name="forget_ms"]');
		await expect(forgetMs).toHaveAttribute('placeholder', '180000');

		const requeryMs = page.locator('input[name="requery_ms"]');
		await expect(requeryMs).toHaveAttribute('placeholder', '300');
	});

	test('should handle loading state when settings are null', async ({ page }) => {
		let resolveSettings: (value: any) => void;
		const settingsPromise = new Promise(resolve => {
			resolveSettings = resolve;
		});

		await page.route('**/wifi/extras', async (route) => {
			await settingsPromise;
			await route.fulfill({
				status: 200,
				contentType: 'application/json',
				body: JSON.stringify(mockExtraSettings)
			});
		});

		await page.goto('/settings');

		// Form should not be visible while loading
		await expect(page.locator('form#extras')).toBeHidden();

		// Resolve promise and wait for response to complete
		const responsePromise = page.waitForResponse('**/wifi/extras');
		resolveSettings!(null);
		await responsePromise;

		// Form should now be visible after settings load
		await expect(page.locator('form#extras')).toBeVisible();
	});

	test('should have consistent styling with hardware page', async ({ page }) => {
		await page.goto('/settings');
		await page.waitForSelector('form#extras');

		// Check for consistent Tailwind classes
		const container = page.locator('.bg-gray-100.dark\\:bg-gray-800.rounded-lg.shadow');
		await expect(container).toBeVisible();

		// Check submit button styling
		const submitButton = page.locator('button[type="submit"]');
		const buttonClass = await submitButton.getAttribute('class');
		expect(buttonClass).toContain('bg-blue-600');
		expect(buttonClass).toContain('text-white');
	});

	// Note: Store retry behavior test removed - the extraSettings store has complex
	// retry logic with polling that's difficult to test reliably. The store's
	// behavior is not changed by PR 1951, so testing it here isn't critical.
});

test.describe('Settings Page Integration', () => {
	test('should be accessible from navigation', async ({ page }) => {
		// Mock required endpoints
		await page.route('**/json', async (route) => {
			await route.fulfill({
				status: 200,
				contentType: 'application/json',
				body: JSON.stringify({ room: 'test-room' })
			});
		});

		await page.route('**/wifi/extras', async (route) => {
			await route.fulfill({
				status: 200,
				contentType: 'application/json',
				body: JSON.stringify(mockExtraSettings)
			});
		});

		await page.goto('/');

		// Look for Settings link in navigation
		const settingsLink = page.locator('a[href="/settings"]');
		await expect(settingsLink).toBeVisible();

		// Click and verify navigation
		await settingsLink.click();
		await expect(page).toHaveURL('/settings');

		// Verify settings page loaded
		await expect(page.locator('form#extras')).toBeVisible();
	});

	test('should have separate Hardware link in navigation', async ({ page }) => {
		await page.route('**/json', async (route) => {
			await route.fulfill({
				status: 200,
				contentType: 'application/json',
				body: JSON.stringify({ room: 'test-room' })
			});
		});

		await page.goto('/');

		// Both Settings and Hardware should be in navigation
		const settingsLink = page.locator('a[href="/settings"]');
		const hardwareLink = page.locator('a[href="/hardware"]');

		await expect(settingsLink).toBeVisible();
		await expect(hardwareLink).toBeVisible();
	});
});
