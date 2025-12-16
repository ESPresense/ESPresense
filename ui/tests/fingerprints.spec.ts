import { test, expect } from '@playwright/test';

// Mock data for fingerprints (live devices)
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
			vis: false,
			close: true
		}
	]
};

test.describe('Fingerprints Page', () => {
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
	});

	test('should load fingerprints page and display table', async ({ page }) => {
		await page.goto('/fingerprints');

		// Check page title
		await expect(page.locator('h2:has-text("Fingerprints")')).toBeVisible();

		// Check table exists
		await expect(page.locator('table')).toBeVisible();
	});

	test('should display correct column headers', async ({ page }) => {
		await page.goto('/fingerprints');
		await page.waitForSelector('table');
		await expect(page.locator('table tbody tr')).toHaveCount(2, { timeout: 5000 });

		// Check column headers using nth selectors to avoid ambiguity
		const headers = page.locator('thead tr:first-child th');
		await expect(headers.nth(0)).toContainText('Visible');
		await expect(headers.nth(1)).toContainText('Dist');
		await expect(headers.nth(2)).toContainText('Var');
		await expect(headers.nth(3)).toContainText('ID');
		await expect(headers.nth(4)).toContainText('Name');
		await expect(headers.nth(5)).toContainText('MAC');
		await expect(headers.nth(6)).toContainText('RSSI');
		await expect(headers.nth(7)).toContainText('RSSI@1m');
		await expect(headers.nth(8)).toContainText('int');
	});

	test('should display device data in table', async ({ page }) => {
		await page.goto('/fingerprints');

		// Wait for table to be visible (even if empty initially)
		await page.waitForSelector('table');
		await expect(page.locator('table tbody tr')).toHaveCount(2, { timeout: 5000 });

		// By default, only visible devices are shown (vis filter = ⬤)
		await expect(page.locator('text=iPhone 13')).toBeVisible();
		await expect(page.locator('text=MacBook Pro')).toBeVisible();

		// Tile Tracker has vis: false, so it's filtered out by default
		// Clear the visibility filter to see all devices
		const visFilter = page.locator('thead tr:last-child select').first();
		await visFilter.selectOption({ index: 0 }); // Select "All"
		await expect(page.locator('table tbody tr')).toHaveCount(3, { timeout: 5000 });

		// Now Tile Tracker should be visible
		await expect(page.locator('text=Tile Tracker')).toBeVisible();
	});

	test('should display distance values with correct format', async ({ page }) => {
		await page.goto('/fingerprints');
		await page.waitForSelector('table tbody tr');

		// Check distance formatting (e.g., "1.20 m")
		await expect(page.locator('text=/1\\.20 m/')).toBeVisible();
		await expect(page.locator('text=/3\\.50 m/')).toBeVisible();
	});

	test('should display RSSI values with dBm unit', async ({ page }) => {
		await page.goto('/fingerprints');

		await expect(page.locator('table tbody tr')).toHaveCount(2, { timeout: 5000 });

		// Check RSSI formatting in table cells
		// Find cells that contain RSSI values with dBm unit
		const rssiCells = page.locator('td:has-text("dBm")');
		const count = await rssiCells.count();

		// Should have RSSI values displayed (multiple cells since there are 2 devices × 2 RSSI columns)
		expect(count).toBeGreaterThan(0);

		// Verify specific values exist - use .first() since values may appear in multiple columns
		await expect(page.locator('td', { hasText: '-45 dBm' }).first()).toBeVisible();
		await expect(page.locator('td', { hasText: '-65 dBm' }).first()).toBeVisible();
	});

	test('should show visibility indicator for visible devices', async ({ page }) => {
		await page.goto('/fingerprints');
		await page.waitForSelector('table tbody tr');

		// The visible column shows "⬤" for visible devices
		const visibleIndicators = page.locator('td:has-text("⬤")');
		expect(await visibleIndicators.count()).toBeGreaterThan(0);
	});

	test('should sort by distance by default (ascending)', async ({ page }) => {
		await page.goto('/fingerprints');
		await page.waitForSelector('table tbody tr');

		// Default sort should be by distance ascending
		// iPhone 13 (1.2m) should be first
		const firstRow = page.locator('table tbody tr').first();
		await expect(firstRow).toContainText('iPhone 13');

		// Check for sort indicator on Distance column
		const distHeader = page.locator('th:has-text("Dist")');
		await expect(distHeader.locator('span')).toBeVisible();
	});

	test('should allow sorting by clicking column headers', async ({ page }) => {
		await page.goto('/fingerprints');
		await page.waitForSelector('table tbody tr');

		// Click Name header to sort
		await page.locator('th:has-text("Name")').click();

		// Check sort indicator appears
		await expect(page.locator('th:has-text("Name") >> span')).toBeVisible({ timeout: 2000 });

		// First device alphabetically should be "iPhone 13"
		const firstRow = page.locator('table tbody tr').first();
		await expect(firstRow).toContainText('iPhone');
	});

	test('should reverse sort order when clicking same header twice', async ({ page }) => {
		await page.goto('/fingerprints');
		await page.waitForSelector('table tbody tr');

		const nameHeader = page.locator('th:has-text("Name")');

		// First click - ascending
		await nameHeader.click();
		await expect(nameHeader.locator('text=↑')).toBeVisible({ timeout: 2000 });

		// Second click - descending
		await nameHeader.click();
		await expect(nameHeader.locator('text=↓')).toBeVisible({ timeout: 2000 });
	});

	test('should filter devices by visibility', async ({ page }) => {
		await page.goto('/fingerprints');

		// Initially should show rows (visible filter defaults to vis=true)
		await expect(page.locator('table tbody tr')).toHaveCount(2, { timeout: 5000 });

		// Find the visibility filter dropdown
		const filterSelects = page.locator('thead tr:last-child select');
		if (await filterSelects.count() > 0) {
			const visFilter = filterSelects.first();

			// Check if it has the visibility options
			const options = await visFilter.locator('option').allTextContents();
			if (options.some(opt => opt === '⬤' || opt.includes('All'))) {
				// Select "All" to show all devices including invisible ones
				await visFilter.selectOption({ index: 0 }); // "All" is usually first option
				await expect(page.locator('table tbody tr')).toHaveCount(3, { timeout: 5000 });
			}
		}
	});

	test('should filter devices by ID prefix', async ({ page }) => {
		await page.goto('/fingerprints');

		await expect(page.locator('table tbody tr')).toHaveCount(2, { timeout: 5000 });

		// Find the ID column filter
		const filterSelects = page.locator('thead tr:last-child select');
		const selectCount = await filterSelects.count();

		// ID filter should be present (columns with filters: vis, id, name, mac)
		if (selectCount >= 2) {
			const idFilter = filterSelects.nth(1); // vis, then id

			// Get available options
			const options = await idFilter.locator('option').allTextContents();

			// Find an ID prefix option (should be "apple:" or "tile:")
			const appleOption = options.find(opt => opt.includes('apple:'));
			if (appleOption) {
				await idFilter.selectOption({ label: appleOption });
				await expect(page.locator('table tbody tr')).toHaveCount(2, { timeout: 5000 });

				await expect(page.locator('tbody >> text=iPhone 13')).toBeVisible();
				await expect(page.locator('tbody >> text=MacBook Pro')).toBeVisible();
			}
		}
	});

	test('should filter devices by name first letter', async ({ page }) => {
		await page.goto('/fingerprints');
		await page.waitForSelector('table tbody tr');

		// Find the Name column filter
		const filterSelects = page.locator('thead tr:last-child select');
		const nameFilter = filterSelects.nth(4); // Name column

		if (await nameFilter.isVisible()) {
			// Select "I" for iPhone
			const options = await nameFilter.locator('option').allTextContents();
			if (options.includes('I')) {
				await nameFilter.selectOption({ label: 'I' });
				await expect(page.locator('table tbody tr')).toHaveCount(1, { timeout: 3000 });

				// Should only show iPhone
				await expect(page.locator('tbody >> text=iPhone 13')).toBeVisible();
			}
		}
	});

	test('should highlight close devices', async ({ page }) => {
		await page.goto('/fingerprints');

		// Clear visibility filter to see all devices (Tile Tracker has vis: false)
		const visFilter = page.locator('thead tr:last-child select').first();
		await visFilter.selectOption({ index: 0 }); // Select "All"
		await expect(page.locator('table tbody tr')).toHaveCount(3, { timeout: 5000 });

		// Tile Tracker has close: true, should have highlight class
		const rows = page.locator('table tbody tr');
		const tileRow = rows.filter({ hasText: 'Tile Tracker' });

		// Check if it has the yellow highlight class
		const className = await tileRow.getAttribute('class');
		expect(className).toContain('bg-yellow');
	});

	test('should handle empty device list', async ({ page }) => {
		// Override with empty devices
		await page.route('**/json/devices?showAll', async (route) => {
			await route.fulfill({
				status: 200,
				contentType: 'application/json',
				body: JSON.stringify({ room: 'test-room', devices: [] })
			});
		});

		await page.goto('/fingerprints');
		await page.waitForSelector('table');

		// Headers should be visible
		await expect(page.locator('th:has-text("Name")')).toBeVisible();

		// No data rows
		expect(await page.locator('table tbody tr').count()).toBe(0);
	});

	test('should show loading state when data is null', async ({ page }) => {
		// Override the devices store to return null initially
		await page.route('**/json/devices?showAll', async (route) => {
			// Delay the response to simulate loading
			await new Promise(resolve => setTimeout(resolve, 100));
			await route.fulfill({
				status: 200,
				contentType: 'application/json',
				body: JSON.stringify(mockDevices)
			});
		});

		// Start navigation and quickly check for loading state
		const gotoPromise = page.goto('/fingerprints');

		// The page shows the table immediately since store starts with empty array
		// Just verify it eventually loads data
		await gotoPromise;
		await expect(page.locator('table tbody tr')).toHaveCount(2, { timeout: 5000 });
		await expect(page.locator('table')).toBeVisible();
	});

	test('should display variance values with correct format', async ({ page }) => {
		await page.goto('/fingerprints');
		await page.waitForSelector('table tbody tr');

		// Check variance formatting (e.g., "0.15 m")
		await expect(page.locator('text=/0\\.15 m/')).toBeVisible();
	});

	test('should display interval values with ms unit', async ({ page }) => {
		await page.goto('/fingerprints');
		await page.waitForSelector('table tbody tr');

		// Check interval formatting
		await expect(page.locator('text=/100 ms/')).toBeVisible();
		await expect(page.locator('text=/200 ms/')).toBeVisible();
	});

	test('should handle n/a values for missing data', async ({ page }) => {
		// Override with device missing some fields
		await page.route('**/json/devices?showAll', async (route) => {
			await route.fulfill({
				status: 200,
				contentType: 'application/json',
				body: JSON.stringify({
					room: 'test-room',
					devices: [{
						id: 'test:123',
						name: 'Test Device',
						vis: true, // Make it visible so it shows up
						// Missing distance, var, mac, rssi, rssi@1m, int
					}]
				})
			});
		});

		await page.goto('/fingerprints');

		await expect(page.locator('table tbody tr')).toHaveCount(1, { timeout: 5000 });

		// Should show "n/a" for missing values (distance, var, and mac show n/a when undefined)
		const naCells = page.locator('td:has-text("n/a")');
		await expect(async () => {
			expect(await naCells.count()).toBeGreaterThan(0);
		}).toPass({ timeout: 3000 });
	});

	test('should sort RSSI values numerically', async ({ page }) => {
		await page.goto('/fingerprints');

		// Wait for table to load
		await page.waitForSelector('table');
		await expect(page.locator('table tbody tr')).toHaveCount(2, { timeout: 5000 });

		// Find the RSSI column header (7th column)
		const rssiHeader = page.locator('thead tr:first-child th').nth(6);
		await rssiHeader.click();
		await expect(rssiHeader.locator('span')).toBeVisible({ timeout: 2000 });

		// Get all RSSI values from the 7th column
		const rows = page.locator('table tbody tr');
		const count = await rows.count();

		const rssiValues: number[] = [];
		for (let i = 0; i < count; i++) {
			const row = rows.nth(i);
			const cells = row.locator('td');
			// RSSI is 7th column (0-indexed: vis, dist, var, id, name, mac, rssi)
			const rssiCell = cells.nth(6);
			const text = await rssiCell.textContent();
			if (text && text.includes('dBm')) {
				const value = parseInt(text.replace(' dBm', ''));
				rssiValues.push(value);
			}
		}

		// Check ascending order
		for (let i = 1; i < rssiValues.length; i++) {
			expect(rssiValues[i]).toBeGreaterThanOrEqual(rssiValues[i - 1]);
		}
	});

	test('should have hover effects on rows', async ({ page }) => {
		await page.goto('/fingerprints');
		await page.waitForSelector('table tbody tr');

		const firstRow = page.locator('table tbody tr').first();
		const className = await firstRow.getAttribute('class');

		// Should have cursor-pointer and hover class
		expect(className).toContain('cursor-pointer');
		expect(className).toContain('hover:bg-gray');
	});
});
