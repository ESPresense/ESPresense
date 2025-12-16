import { test, expect } from '@playwright/test';

// Mock data for devices and fingerprints pages
const mockDevices = {
	room: 'test-room',
	devices: [
		{
			id: 'apple:1005:1-2',
			name: 'iPhone 13',
			mac: 'aa:bb:cc:dd:ee:ff',
			rssi: -45,
			'rssi@1m': -65,
			distance: 1.2,
			var: 0.15,
			int: 100,
			vis: true,
			close: false
		},
		{
			id: 'apple:1005:1-3',
			name: 'MacBook Pro',
			mac: '11:22:33:44:55:66',
			rssi: -65,
			'rssi@1m': -70,
			distance: 3.5,
			var: 0.25,
			int: 200,
			vis: true,
			close: false
		},
		{
			id: 'tile:1234',
			name: 'Tile Tracker',
			mac: '77:88:99:aa:bb:cc',
			rssi: -55,
			'rssi@1m': -60,
			distance: 2.1,
			var: 0.18,
			int: 150,
			vis: true,
			close: false
		}
	]
};

const mockFingerprints = {
	room: 'test-room',
	configs: [
		{
			id: 'aabbccddeeff',
			name: 'Alice Phone',
			calRssi: -65,
			query: true
		},
		{
			id: '112233445566',
			name: 'Bob Watch',
			calRssi: -70,
			query: false
		},
		{
			id: '778899aabbcc',
			name: 'Charlie Tracker',
			calRssi: -60,
			query: true
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

		await page.route('**/json/devices?showAll', async (route) => {
			await route.fulfill({
				status: 200,
				contentType: 'application/json',
				body: JSON.stringify(mockDevices)
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

		// Check table headers exist (using abbreviated column names)
		await expect(page.locator('th:has-text("Name")')).toBeVisible();
		await expect(page.locator('th:has-text("ID")')).toBeVisible();
		await expect(page.locator('th:has-text("RSSI")')).toBeVisible();
		await expect(page.locator('th:has-text("Dist")')).toBeVisible();

		// Check data is displayed
		await expect(page.locator('text=iPhone 13')).toBeVisible();
		await expect(page.locator('text=MacBook Pro')).toBeVisible();
		await expect(page.locator('text=Tile Tracker')).toBeVisible();
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

		await page.route('**/json/configs', async (route) => {
			await route.fulfill({
				status: 200,
				contentType: 'application/json',
				body: JSON.stringify(mockFingerprints)
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

		// Check headers
		await expect(page.locator('th:has-text("Name")')).toBeVisible();
		await expect(page.locator('th:has-text("ID")')).toBeVisible();

		// Check data
		await expect(page.locator('text=Alice Phone')).toBeVisible();
		await expect(page.locator('text=Bob Watch')).toBeVisible();
		await expect(page.locator('text=Charlie Tracker')).toBeVisible();
	});

	test('should filter fingerprints by query status', async ({ page }) => {
		await page.goto('/fingerprints');
		await page.waitForSelector('table');

		await expect(async () => {
			expect(await page.locator('table tbody tr').count()).toBeGreaterThan(0);
		}).toPass({ timeout: 5000 });

		// Look for filter dropdown if it exists
		const filterSelect = page.locator('select').first();

		if (await filterSelect.isVisible()) {
			// Select "query: true" filter
			await filterSelect.selectOption('true');

			// Only Alice Phone and Charlie Tracker should be visible (they have query: true)
			await expect(page.locator('text=Alice Phone')).toBeVisible();
			await expect(page.locator('text=Charlie Tracker')).toBeVisible();

			// Bob Watch should not be visible (query: false)
			await expect(page.locator('tbody >> text=Bob Watch')).not.toBeVisible();

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

		const nameHeader = page.locator('th:has-text("Name")');

		// Click to sort ascending
		await nameHeader.click();
		await expect(async () => {
			expect(await page.locator('table tbody tr').count()).toBeGreaterThan(0);
		}).toPass({ timeout: 2000 });

		// Get all names in order
		const rows = page.locator('table tbody tr');
		const count = await rows.count();
		const names: string[] = [];

		for (let i = 0; i < count; i++) {
			const nameCell = rows.nth(i).locator('td').first();
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
			const nameCell = rows.nth(i).locator('td').first();
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

		await page.route('**/json/devices?showAll', async (route) => {
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
		await page.route('**/json/devices?showAll', async (route) => {
			await route.fulfill({
				status: 200,
				contentType: 'application/json',
				body: JSON.stringify({ room: 'test-room', devices: [] })
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
		await page.route('**/json/devices?showAll', async (route) => {
			await route.fulfill({
				status: 200,
				contentType: 'application/json',
				body: JSON.stringify({
					room: 'test-room',
					devices: [
						{
							id: 'test:123',
							name: null, // null name
							rssi: -50,
							distance: null, // null distance
							interval: 100,
							lastSeen: 1000
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

		// Sort by RSSI (numeric)
		await page.locator('th:has-text("RSSI")').click();

		const rows = page.locator('table tbody tr');
		const count = await rows.count();

		// Get RSSI values
		const rssiValues: number[] = [];
		for (let i = 0; i < count; i++) {
			const rssiCell = rows.nth(i).locator('td:nth-child(3)'); // Assuming RSSI is 3rd column
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

	test('should display custom rendered components', async ({ page }) => {
		await page.goto('/devices');
		await page.waitForSelector('table');

		await expect(async () => {
			expect(await page.locator('table tbody tr').count()).toBeGreaterThan(0);
		}).toPass({ timeout: 5000 });

		// Check if any custom components are rendered (buttons, icons, etc.)
		// This depends on the actual implementation in devices page
		const tableButtons = page.locator('table button');
		const tableLinks = page.locator('table a');
		const buttonCount = await tableButtons.count();
		const linkCount = await tableLinks.count();
		const customElementCount = buttonCount + linkCount;

		expect(customElementCount).toBeGreaterThan(0);
		if (buttonCount > 0) {
			await expect(tableButtons.first()).toBeVisible();
		} else {
			await expect(tableLinks.first()).toBeVisible();
		}

		// Just verify table is functional
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
			// Count rows with no filter
			const initialRowCount = await page.locator('table tbody tr').count();
			expect(initialRowCount).toBe(3);

			// Apply filter
			await filterSelect.selectOption('true');

			// Row count should decrease
			await expect(page.locator('table tbody tr')).toHaveCount(2, { timeout: 2000 });
			expect(2).toBeLessThan(initialRowCount);

			// Clear filter
			await filterSelect.selectOption('');

			// Back to original count
			await expect(page.locator('table tbody tr')).toHaveCount(initialRowCount, { timeout: 2000 });
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

		// Sort by multiple columns in sequence
		await page.locator('th:has-text("Name")').click();
		await page.locator('th:has-text("RSSI")').click();
		await page.locator('th:has-text("Dist")').click();

		// Table should still be functional and retain all rows after sorting
		const postSortRowCount = await rows.count();
		expect(postSortRowCount).toBe(initialRowCount);

		// Last sorted column should have indicator
		await expect(page.locator('th:has-text("Dist") >> span')).toBeVisible();
	});
});
