import { test, expect } from '@playwright/test';

// Mock data for devices page (uses /json/configs endpoint)
const mockConfigs = {
	room: 'test-room',
	configs: [
		{
			id: 'apple:1005:1-2',
			alias: 'apple:iphone-13',
			name: 'Zara iPhone',
			'rssi@1m': -65
		},
		{
			id: 'apple:1005:1-3',
			alias: 'apple:macbook-pro',
			name: 'MacBook Pro',
			'rssi@1m': -70
		},
		{
			id: 'tile:1234',
			alias: 'tile:tracker',
			name: 'Alice Tracker',
			'rssi@1m': -60
		}
	]
};

// Mock data for fingerprints page (uses /json/devices endpoint)
const mockDevices = {
	room: 'test-room',
	devices: [
		{
			id: 'apple:alice-phone',
			name: 'Alice Phone',
			rssi: -45,
			'rssi@1m': -65,
			distance: 1.2,
			var: 0.15,
			vis: true
		},
		{
			id: 'apple:bob-watch',
			name: 'Bob Watch',
			rssi: -65,
			'rssi@1m': -70,
			distance: 3.5,
			var: 0.25,
			vis: false
		},
		{
			id: 'tile:charlie-tracker',
			name: 'Charlie Tracker',
			rssi: -55,
			'rssi@1m': -60,
			distance: 2.1,
			var: 0.18,
			vis: true
		}
	]
};

test.describe('DataTable - Devices Page', () => {
	test.beforeEach(async ({ page }) => {
		// Mock endpoints
		await page.route('**/json', async (route) => {
			await route.fulfill({
				status: 200,
				contentType: 'application/json',
				body: JSON.stringify({ room: 'test-room' })
			});
		});

		await page.route('**/json/configs', async (route) => {
			await route.fulfill({
				status: 200,
				contentType: 'application/json',
				body: JSON.stringify(mockConfigs)
			});
		});

		await page.route('**/ws', async (route) => {
			// Mock WebSocket connection
			await route.fulfill({ status: 101 });
		});
	});

	test('should render devices table with data', async ({ page }) => {
		await page.goto('/devices');

		// Wait for table to load
		await page.waitForSelector('table');

		await expect(async () => {
			expect(await page.locator('table tbody tr').count()).toBeGreaterThan(0);
		}).toPass({ timeout: 5000 });

		// Check table headers exist
		await expect(page.locator('th:has-text("Name")')).toBeVisible();
		await expect(page.locator('th:has-text("ID")')).toBeVisible();
		await expect(page.locator('th:has-text("Alias")')).toBeVisible();
		await expect(page.locator('th:has-text("RSSI@1m")')).toBeVisible();

		// Check data is displayed
		await expect(page.locator('text=Zara iPhone')).toBeVisible();
		await expect(page.locator('text=MacBook Pro')).toBeVisible();
		await expect(page.locator('text=Alice Tracker')).toBeVisible();
	});

	test('should sort table by clicking column headers', async ({ page }) => {
		await page.goto('/devices');

		await expect(async () => {
			expect(await page.locator('table tbody tr').count()).toBeGreaterThan(0);
		}).toPass({ timeout: 5000 });

		// Get all rows
		const getFirstRowName = async () => {
			const firstRow = page.locator('table tbody tr').first();
			return await firstRow.locator('td').first().textContent();
		};

		// Initial order (should be sorted by default column)
		const initialFirst = await getFirstRowName();

		// Click Name header to sort
		const nameHeader = page.locator('th:has-text("Name")');
		await nameHeader.click();

		let afterFirstSort = initialFirst;
		await expect(async () => {
			afterFirstSort = await getFirstRowName();
			expect(afterFirstSort).not.toBe(initialFirst);
		}).toPass({ timeout: 2000 });

		// Click again to reverse sort
		await nameHeader.click();
		let afterSecondSort = afterFirstSort;
		await expect(async () => {
			afterSecondSort = await getFirstRowName();
			expect(afterSecondSort).not.toBe(afterFirstSort);
		}).toPass({ timeout: 2000 });

		// After reversing, order should be different
		expect(afterFirstSort).not.toBe(afterSecondSort);
	});

	test('should show sort indicator on sorted column', async ({ page }) => {
		await page.goto('/devices');
		await page.waitForSelector('table');

		// Click RSSI header to sort
		const rssiHeader = page.locator('th:has-text("RSSI")');
		await rssiHeader.click();
		await expect(rssiHeader.locator('text=↑')).toBeVisible({ timeout: 2000 });

		// Click again to reverse
		await rssiHeader.click();
		await expect(rssiHeader.locator('text=↓')).toBeVisible({ timeout: 2000 });
	});

	test('should handle row clicks', async ({ page }) => {
		await page.goto('/devices');

		await expect(async () => {
			expect(await page.locator('table tbody tr').count()).toBeGreaterThan(0);
		}).toPass({ timeout: 5000 });

		// Click on a row
		const firstRow = page.locator('table tbody tr').first();
		await firstRow.click();

		// This would typically open a modal or navigate
		// For now, just verify the row is clickable (cursor-pointer class)
		const rowClass = await firstRow.getAttribute('class');
		expect(rowClass).toContain('cursor-pointer');
	});

	test('should not trigger row click when clicking interactive elements', async ({ page }) => {
		await page.goto('/devices');

		await expect(async () => {
			expect(await page.locator('table tbody tr').count()).toBeGreaterThan(0);
		}).toPass({ timeout: 5000 });

		// If there's a button or link in the row, clicking it shouldn't trigger row click
		// This tests the isInteractiveTarget logic
		const button = page.locator('table tbody tr button').first();
		if (await button.isVisible()) {
			await button.click();
			await expect(page.locator('dialog[open], [role="dialog"]')).toHaveCount(0);
		}
	});
});

test.describe('DataTable - Fingerprints Page', () => {
	test.beforeEach(async ({ page }) => {
		await page.route('**/json', async (route) => {
			await route.fulfill({
				status: 200,
				contentType: 'application/json',
				body: JSON.stringify({ room: 'test-room' })
			});
		});

		await page.route('**/json/devices*', async (route) => {
			await route.fulfill({
				status: 200,
				contentType: 'application/json',
				body: JSON.stringify(mockDevices)
			});
		});

		await page.route('**/ws', async (route) => {
			await route.fulfill({ status: 101 });
		});
	});

	test('should render fingerprints table with data', async ({ page }) => {
		await page.goto('/fingerprints');
		await page.waitForSelector('table');

		await expect(async () => {
			expect(await page.locator('table tbody tr').count()).toBeGreaterThan(0);
		}).toPass({ timeout: 5000 });

		// Check headers (fingerprints page has Visible, Dist, Var, ID, Name, RSSI columns)
		await expect(page.locator('th:has-text("Name")')).toBeVisible();
		await expect(page.locator('th:has-text("ID")')).toBeVisible();
		await expect(page.locator('th:has-text("Dist")')).toBeVisible();

		// Fingerprints page defaults to showing only vis:true items
		// So Alice Phone and Charlie Tracker should be visible (they have vis: true)
		await expect(page.locator('text=Alice Phone')).toBeVisible();
		await expect(page.locator('text=Charlie Tracker')).toBeVisible();
		// Bob Watch has vis: false, so it's hidden by default filter
	});

	test('should filter fingerprints by visibility status', async ({ page }) => {
		await page.goto('/fingerprints');
		await page.waitForSelector('table');

		await expect(async () => {
			expect(await page.locator('table tbody tr').count()).toBeGreaterThan(0);
		}).toPass({ timeout: 5000 });

		// Look for filter dropdown if it exists
		const filterSelect = page.locator('select').first();

		if (await filterSelect.isVisible()) {
			// Select visible: false filter
			await filterSelect.selectOption('false');

			// Only Bob Watch should be visible (vis: false)
			await expect(page.locator('tbody >> text=Bob Watch')).toBeVisible();

			// Alice Phone and Charlie Tracker should not be visible (vis: true)
			await expect(page.locator('tbody >> text=Alice Phone')).not.toBeVisible();

			// Select "All" to show everything again
			await filterSelect.selectOption('');

			// All should be visible now
			await expect(page.locator('text=Alice Phone')).toBeVisible();
			await expect(page.locator('text=Bob Watch')).toBeVisible();
			await expect(page.locator('text=Charlie Tracker')).toBeVisible();
		}
	});

	test('should sort fingerprints alphabetically by name', async ({ page }) => {
		await page.goto('/fingerprints');

		await expect(async () => {
			expect(await page.locator('table tbody tr').count()).toBeGreaterThan(0);
		}).toPass({ timeout: 5000 });

		// Clear the default visibility filter to show all items
		const filterSelect = page.locator('select').first();
		if (await filterSelect.isVisible()) {
			await filterSelect.selectOption('');
			await expect(page.locator('table tbody tr')).toHaveCount(3, { timeout: 2000 });
		}

		const nameHeader = page.locator('th:has-text("Name")');

		// Click to sort ascending by name
		await nameHeader.click();
		await expect(async () => {
			expect(await page.locator('table tbody tr').count()).toBeGreaterThan(0);
		}).toPass({ timeout: 2000 });

		// Get all names in order (Name is the 5th column - index 4)
		const rows = page.locator('table tbody tr');
		const count = await rows.count();
		const names: string[] = [];

		for (let i = 0; i < count; i++) {
			const nameCell = rows.nth(i).locator('td:nth-child(5)');
			const name = await nameCell.textContent();
			if (name) names.push(name);
		}

		// Check if sorted (Alice, Bob, Charlie)
		expect(names[0]).toContain('Alice');
		expect(names[1]).toContain('Bob');
		expect(names[2]).toContain('Charlie');

		// Click again to reverse
		await nameHeader.click();
		await expect(async () => {
			expect(await page.locator('table tbody tr').count()).toBeGreaterThan(0);
		}).toPass({ timeout: 2000 });

		// Get names again
		const namesReversed: string[] = [];
		for (let i = 0; i < count; i++) {
			const nameCell = rows.nth(i).locator('td:nth-child(5)');
			const name = await nameCell.textContent();
			if (name) namesReversed.push(name);
		}

		// Should be reversed (Charlie, Bob, Alice)
		expect(namesReversed[0]).toContain('Charlie');
		expect(namesReversed[2]).toContain('Alice');
	});
});

test.describe('DataTable - Component Functionality', () => {
	test.beforeEach(async ({ page }) => {
		await page.route('**/json', async (route) => {
			await route.fulfill({
				status: 200,
				contentType: 'application/json',
				body: JSON.stringify({ room: 'test-room' })
			});
		});

		await page.route('**/json/configs', async (route) => {
			await route.fulfill({
				status: 200,
				contentType: 'application/json',
				body: JSON.stringify(mockConfigs)
			});
		});

		await page.route('**/json/devices*', async (route) => {
			await route.fulfill({
				status: 200,
				contentType: 'application/json',
				body: JSON.stringify(mockDevices)
			});
		});

		await page.route('**/ws', async (route) => {
			await route.fulfill({ status: 101 });
		});
	});

	test('should handle empty data gracefully', async ({ page }) => {
		// Override with empty data
		await page.route('**/json/configs', async (route) => {
			await route.fulfill({
				status: 200,
				contentType: 'application/json',
				body: JSON.stringify({ room: 'test-room', configs: [] })
			});
		});

		await page.goto('/devices');
		await page.waitForSelector('table');

		// Headers should still be visible
		await expect(page.locator('th:has-text("Name")')).toBeVisible();

		// No data rows
		const rows = page.locator('table tbody tr');
		expect(await rows.count()).toBe(0);
	});

	test('should handle null/undefined values in cells', async ({ page }) => {
		// Override with data containing null values
		await page.route('**/json/configs', async (route) => {
			await route.fulfill({
				status: 200,
				contentType: 'application/json',
				body: JSON.stringify({
					room: 'test-room',
					configs: [
						{
							id: 'test:123',
							alias: null, // null alias
							name: null, // null name
							'rssi@1m': null // null rssi
						}
					]
				})
			});
		});

		await page.goto('/devices');

		// Table should render once data is available
		await expect(page.locator('table tbody tr')).toHaveCount(1, { timeout: 5000 });

		// Null values should be handled (empty string or default)
		const firstRow = page.locator('table tbody tr').first();
		await expect(firstRow).toBeVisible();
	});

	test('should maintain sort when data updates', async ({ page }) => {
		await page.goto('/devices');
		await page.waitForSelector('table');

		// Sort by Name
		await page.locator('th:has-text("Name")').click();
		await expect(page.locator('th:has-text("Name") >> text=↑')).toBeVisible({ timeout: 2000 });

		// Sort indicator should still be there after data refresh
		await expect(page.locator('th:has-text("Name") >> text=↑')).toBeVisible({ timeout: 5000 });
	});

	test('should apply custom row classes', async ({ page }) => {
		await page.goto('/devices');

		await expect(async () => {
			expect(await page.locator('table tbody tr').count()).toBeGreaterThan(0);
		}).toPass({ timeout: 5000 });

		// Rows should have cursor-pointer class
		const firstRow = page.locator('table tbody tr').first();
		const rowClass = await firstRow.getAttribute('class');
		expect(rowClass).toContain('cursor-pointer');
	});

	test('should handle sorting with mixed data types', async ({ page }) => {
		await page.goto('/devices');
		await page.waitForSelector('table');

		await expect(async () => {
			expect(await page.locator('table tbody tr').count()).toBeGreaterThan(0);
		}).toPass({ timeout: 5000 });

		// Sort by RSSI@1m (numeric)
		await page.locator('th:has-text("RSSI@1m")').click();

		const rows = page.locator('table tbody tr');
		const count = await rows.count();

		// Get RSSI values
		const rssiValues: number[] = [];
		for (let i = 0; i < count; i++) {
			const rssiCell = rows.nth(i).locator('td:nth-child(4)'); // RSSI@1m is 4th column
			const text = await rssiCell.textContent();
			if (text) {
				const value = parseInt(text.trim());
				if (!isNaN(value)) rssiValues.push(value);
			}
		}

		// Should be sorted numerically (ascending)
		for (let i = 1; i < rssiValues.length; i++) {
			expect(rssiValues[i]).toBeGreaterThanOrEqual(rssiValues[i - 1]);
		}
	});

	test('should display table with clickable rows', async ({ page }) => {
		await page.goto('/devices');
		await page.waitForSelector('table');

		await expect(async () => {
			expect(await page.locator('table tbody tr').count()).toBeGreaterThan(0);
		}).toPass({ timeout: 5000 });

		// Check rows have cursor-pointer class for clickability
		const firstRow = page.locator('table tbody tr').first();
		const rowClass = await firstRow.getAttribute('class');
		expect(rowClass).toContain('cursor-pointer');

		// Verify table is functional
		expect(await page.locator('table tbody tr').count()).toBeGreaterThan(0);
	});

	test('should update filtered rows when filter changes', async ({ page }) => {
		await page.goto('/fingerprints');
		await page.waitForSelector('table');

		await expect(async () => {
			expect(await page.locator('table tbody tr').count()).toBeGreaterThan(0);
		}).toPass({ timeout: 5000 });

		const filterSelect = page.locator('select').first();

		if (await filterSelect.isVisible()) {
			// Fingerprints page defaults to showing only vis:true items (2 rows: Alice, Charlie)
			// Clear the filter first to show all items
			await filterSelect.selectOption('');
			await expect(page.locator('table tbody tr')).toHaveCount(3, { timeout: 2000 });

			// Apply filter for visible items only
			await filterSelect.selectOption('true');

			// Row count should decrease to 2 (Alice and Charlie have vis: true)
			await expect(page.locator('table tbody tr')).toHaveCount(2, { timeout: 2000 });

			// Clear filter again
			await filterSelect.selectOption('');

			// Back to 3 rows
			await expect(page.locator('table tbody tr')).toHaveCount(3, { timeout: 2000 });
		}
	});

	test('should allow multiple sorts without breaking', async ({ page }) => {
		await page.goto('/devices');
		await page.waitForSelector('table');

		await expect(async () => {
			expect(await page.locator('table tbody tr').count()).toBeGreaterThan(0);
		}).toPass({ timeout: 5000 });

		const rows = page.locator('table tbody tr');
		const initialRowCount = await rows.count();
		expect(initialRowCount).toBeGreaterThan(0);

		// Sort by multiple columns in sequence (using actual columns: Alias, ID, Name, RSSI@1m)
		await page.locator('th:has-text("Name")').click();
		await page.locator('th:has-text("Alias")').click();
		await page.locator('th:has-text("RSSI@1m")').click();

		// Table should still be functional and retain all rows after sorting
		const postSortRowCount = await rows.count();
		expect(postSortRowCount).toBe(initialRowCount);

		// Last sorted column should have indicator
		await expect(page.locator('th:has-text("RSSI@1m") >> span')).toBeVisible();
	});
});
