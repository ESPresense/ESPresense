import { test, expect, type Page } from '@playwright/test';

// The device being looked for, plus two bystanders that drift a little whatever the user does.
let keysNear = true;
let tick = 0;
/** what else the node sees: a twin that moves with the tile, an IRK phone whose MAC rotates, or a tile the node never flags close */
let variant: 'default' | 'twin' | 'rotate' | 'noflag' = 'default';
function devicesBody() {
	tick++;
	const wobble = tick % 2 ? 1 : -1;
	const extra =
		variant === 'twin'
			? [{ id: 'e5e6de405d6b', mac: 'e5e6de405d6b', rssi: keysNear ? -45 : -81, distance: keysNear ? 0.2 : 6, vis: true, close: keysNear }]
			: variant === 'rotate'
				? [{ id: 'irk:c4be', name: 'Phone', mac: tick % 2 ? 'aaaaaaaaaaaa' : 'bbbbbbbbbbbb', rssi: keysNear ? -46 : -80, distance: keysNear ? 0.3 : 6, vis: true, close: keysNear }]
				: [];
	return {
		room: 'office',
		devices: [
			...extra,
			// In the rotate variant the tile sits this one out, so only the phone follows.
			{ id: 'tile:c1d2e3f4a5b6', name: 'Tile', mac: 'c1d2e3f4a5b6', rssi: keysNear && variant !== 'rotate' ? -44 : -82, distance: keysNear ? 0.2 : 6, vis: true, close: variant === 'noflag' || variant === 'rotate' ? false : keysNear },
			{ id: 'sd:0x181a', mac: 'aabbccddeeff', rssi: -70 + wobble, distance: 3.1, vis: true, close: false },
			{ id: 'apple:1005:9-26', mac: '4a1b2c3d4e5f', rssi: -66 - wobble, distance: 2.4, vis: true, close: false },
			// Configured by MAC with an alias: the node reports the alias as its id.
			{ id: 'keys:car', name: 'Car Keys', mac: 'd1e2f3a4b5c6', rssi: -75, distance: 5, vis: true, close: false }
		]
	};
}

async function mockNode(page: Page) {
	keysNear = true;
	tick = 0;
	variant = 'default';
	const posts: unknown[] = [];
	await page.route('**/json', (r) => r.fulfill({ json: { room: 'office' } }));
	await page.route('**/json/devices?showAll', (r) => r.fulfill({ json: devicesBody() }));
	await page.route('**/json/configs', async (r) => {
		if (r.request().method() === 'POST') {
			posts.push(r.request().postDataJSON());
			return r.fulfill({ json: { success: true } });
		}
		return r.fulfill({
			json: {
				room: 'office',
				configs: [
					{ id: 'irk:0011', alias: 'phone:darrell', name: 'Darrell Phone', 'rssi@1m': -59 },
					{ id: 'd1e2f3a4b5c6', alias: 'keys:car', name: 'Car Keys' }
				]
			}
		});
	});
	return posts;
}

/** load a page and wait for hydration: the room name in the title comes from the app's own fetch */
async function open(page: Page, path: string) {
	await page.goto(path);
	await expect(page).toHaveTitle(/office/);
}

const mod = process.platform === 'darwin' ? 'Meta' : 'Control';

test.describe('Command palette', () => {
	test('Cmd/Ctrl-K opens it and a page result navigates', async ({ page }) => {
		await mockNode(page);
		await open(page, '/');
		await page.keyboard.press(`${mod}+k`);
		const box = page.getByRole('dialog').getByRole('combobox');
		await expect(box).toBeFocused();
		await box.fill('fing');
		await expect(page.getByRole('option').first()).toContainText('Fingerprints');
		await page.keyboard.press('Enter');
		await expect(page).toHaveURL(/\/fingerprints/);
		await expect(page.getByRole('dialog')).toHaveCount(0);
	});

	test('Escape closes it', async ({ page }) => {
		await mockNode(page);
		await open(page, '/');
		await page.getByRole('button', { name: /Search or find/ }).click();
		await expect(page.getByRole('dialog')).toBeVisible();
		await page.keyboard.press('Escape');
		await expect(page.getByRole('dialog')).toHaveCount(0);
	});

	test('finds configured devices and saves a new name', async ({ page }) => {
		const posts = await mockNode(page);
		await open(page, '/');
		await page.keyboard.press(`${mod}+k`);
		await page.getByRole('dialog').getByRole('combobox').fill('darrell');
		await expect(page.getByRole('option').first()).toContainText('Darrell Phone');
		await page.keyboard.press('Enter');
		const name = page.getByRole('textbox', { name: 'Name', exact: true });
		await expect(name).toHaveValue('Darrell Phone');
		await expect(page.getByLabel('ID to report')).toHaveValue('phone:darrell');
		await name.fill('Darrell iPhone');
		await page.getByRole('button', { name: 'Save' }).click();
		await expect(page.getByRole('heading', { name: 'Saved' })).toBeVisible();
		expect(posts).toEqual([{ id: 'irk:0011', alias: 'phone:darrell', name: 'Darrell iPhone', 'rssi@1m': -59 }]);
	});

	test('renaming an aliased fingerprint saves under its MAC key, not the alias', async ({ page }) => {
		const posts = await mockNode(page);
		const configsLoaded = page.waitForResponse('**/json/configs');
		await open(page, '/fingerprints');
		await configsLoaded;
		await page.getByRole('cell', { name: 'keys:car' }).click();
		await expect(page.getByLabel('ID to report')).toHaveValue('keys:car');
		await page.getByRole('textbox', { name: 'Name', exact: true }).fill('Car Keys (spare)');
		await page.getByRole('button', { name: 'Save' }).click();
		await expect(page.getByRole('heading', { name: 'Saved' })).toBeVisible();
		expect(posts).toEqual([{ id: 'd1e2f3a4b5c6', alias: 'keys:car', name: 'Car Keys (spare)' }]);
	});

	test('a row clicked before configs load still saves under the MAC key', async ({ page }) => {
		const posts = await mockNode(page);
		let release!: () => void;
		const loaded = new Promise<void>((r) => (release = r));
		await page.route('**/json/configs', async (r) => {
			if (r.request().method() !== 'GET') return r.fallback();
			await loaded;
			await r.fulfill({ json: { room: 'office', configs: [{ id: 'd1e2f3a4b5c6', alias: 'keys:car', name: 'Car Keys', 'rssi@1m': -62 }] } });
		});
		await open(page, '/fingerprints');
		await page.getByRole('cell', { name: 'keys:car' }).click();
		// Until the configs load, the alias can't be told from a raw ID, so Save waits (#2623).
		await page.getByRole('textbox', { name: 'Name', exact: true }).fill('Car Keys (spare)');
		await expect(page.getByRole('button', { name: 'Save' })).toBeDisabled();
		const configsLoaded = page.waitForResponse('**/json/configs');
		release();
		await configsLoaded;
		await page.getByRole('textbox', { name: 'Name', exact: true }).fill('Car Keys (spare)');
		await page.getByLabel('ID to report').fill('keys:car');
		await page.getByRole('button', { name: 'Save' }).click();
		await expect(page.getByRole('heading', { name: 'Saved' })).toBeVisible();
		expect(posts).toEqual([{ id: 'd1e2f3a4b5c6', alias: 'keys:car', name: 'Car Keys (spare)', 'rssi@1m': -62 }]);
	});

	test('a closed palette does not poll /json/devices', async ({ page }) => {
		await mockNode(page);
		let polls = 0;
		await page.route('**/json/devices?showAll', (r) => {
			polls++;
			return r.fulfill({ json: devicesBody() });
		});
		await open(page, '/settings');
		await page.waitForTimeout(2500);
		expect(polls).toBe(0);
		await page.keyboard.press(`${mod}+k`);
		await expect.poll(() => polls).toBeGreaterThan(0);
	});

	test('Tab stays inside the palette', async ({ page }) => {
		await mockNode(page);
		await open(page, '/');
		await page.keyboard.press(`${mod}+k`);
		await page.getByRole('dialog').getByRole('combobox').fill('darrell');
		await page.keyboard.press('Enter');
		for (let i = 0; i < 8; i++) {
			await page.keyboard.press('Tab');
			expect(await page.evaluate(() => !!document.activeElement?.closest('[role=dialog]'))).toBe(true);
		}
	});

	test('warns when a fingerprint ID will not stick', async ({ page }) => {
		await mockNode(page);
		await open(page, '/fingerprints');
		await page.getByRole('cell', { name: 'apple:1005:9-26' }).click();
		await expect(page.getByTestId('unstable-id')).toBeVisible();
	});

	test('a double click on Next step neither skips a step nor closes the palette', async ({ page }) => {
		await page.clock.install();
		await mockNode(page);
		await open(page, '/fingerprints');
		await page.getByRole('button', { name: 'Find a device' }).click();
		await page.clock.runFor(2000);
		await page.getByRole('button', { name: 'Next step' }).dblclick();
		await expect(page.getByText('Step 2', { exact: true })).toBeVisible();
		await expect(page.getByTestId('find-step')).toHaveAttribute('data-phase', 'far');
	});

	/** run Find steps with the tile where the step asks, until it's found or `max` steps are done */
	async function runFind(page: Page, max: number, nearOn: (step: number) => boolean = () => true) {
		const found = page.getByTestId('find-found');
		for (let s = 1; s <= max && !(await found.isVisible()); s++) {
			const phase = await page.getByTestId('find-step').getAttribute('data-phase');
			keysNear = phase === 'near' && nearOn(s);
			await page.clock.runFor(2000);
			// It can finish mid-step, as soon as the pattern completes: let the poll's render land.
			await page.waitForTimeout(100);
			if (await found.isVisible()) break;
			await page.getByRole('button', { name: 'Next step' }).click();
		}
		return found;
	}

	test('Find forgives a step 1 clicked before the device showed as close', async ({ page }) => {
		await page.clock.install();
		await mockNode(page);
		await open(page, '/fingerprints');
		await page.getByRole('button', { name: 'Find a device' }).click();
		const found = await runFind(page, 3, (s) => s !== 1);
		await expect(found).toHaveCount(0);
		await expect(page.getByTestId('find-progress')).toContainText('Nothing has followed the last 3 moves yet');
		await runFind(page, 2);
		await expect(found).toContainText('tile:c1d2e3f4a5b6');
		await expect(page.getByText('Step 5', { exact: true })).toBeVisible();
	});

	test('Find calls two followers at the same signal one radio, and lets you pick', async ({ page }) => {
		await page.clock.install();
		await mockNode(page);
		variant = 'twin';
		await open(page, '/fingerprints');
		await page.getByRole('button', { name: 'Find a device' }).click();
		const found = await runFind(page, 4);
		await expect(found).toHaveCount(0);
		await expect(page.getByTestId('find-progress')).toContainText('one device advertising under 2 addresses');
		await expect(page.getByTestId('find-candidates').locator('li')).toHaveCount(2);
		await page.getByTestId('find-candidates').getByText('Tile').click();
		await expect(page.getByRole('textbox', { name: 'Name', exact: true })).toHaveValue('Tile');
	});

	test('Find follows an IRK phone across a MAC rotation', async ({ page }) => {
		await page.clock.install();
		await mockNode(page);
		variant = 'rotate';
		await open(page, '/fingerprints');
		await page.getByRole('button', { name: 'Find a device' }).click();
		const found = await runFind(page, 3);
		await expect(found).toContainText('irk:c4be');
		// One row, not one per MAC.
		await expect(page.getByTestId('find-candidates').locator('li')).toHaveCount(1);
	});

	test('Find counts a device at -50 dBm or louder as close even without the close flag', async ({ page }) => {
		await page.clock.install();
		await mockNode(page);
		variant = 'noflag';
		await open(page, '/fingerprints');
		await page.getByRole('button', { name: 'Find a device' }).click();
		const found = await runFind(page, 3);
		await expect(found).toContainText('tile:c1d2e3f4a5b6');
	});

	test('Find picks out the device that follows near and far, then names it', async ({ page }) => {
		await page.clock.install();
		const posts = await mockNode(page);
		await open(page, '/fingerprints');
		await page.getByRole('button', { name: 'Find a device' }).click();
		await expect(page.getByRole('dialog', { name: 'Find a device' })).toBeVisible();

		const found = await runFind(page, 6);
		await expect(found).toContainText('tile:c1d2e3f4a5b6');
		await expect(page.getByTestId('find-candidates').locator('li').first()).toContainText('Tile');

		await page.getByRole('button', { name: 'Name it' }).click();
		await expect(page.getByTestId('unstable-id')).toHaveCount(0);
		await page.getByRole('textbox', { name: 'Name', exact: true }).fill('Kitchen Keys');
		await page.getByLabel('Type').selectOption('keys');
		await expect(page.getByLabel('ID to report')).toHaveAttribute('placeholder', 'keys:kitchen');
		await page.keyboard.press('Enter');
		await expect(page.getByRole('heading', { name: 'Saved' })).toBeVisible();
		expect(posts).toEqual([{ id: 'tile:c1d2e3f4a5b6', alias: 'keys:kitchen', name: 'Kitchen Keys' }]);
	});

	test('the Devices page opens Find, and the device it finds is named from there', async ({ page }) => {
		await page.clock.install();
		const posts = await mockNode(page);
		await open(page, '/devices');
		await page.getByRole('button', { name: 'Find a device' }).click();
		await expect(page.getByRole('dialog', { name: 'Find a device' })).toBeVisible();

		const found = await runFind(page, 6);
		await expect(found).toContainText('tile:c1d2e3f4a5b6');

		await page.getByRole('button', { name: 'Name it' }).click();
		await page.getByRole('textbox', { name: 'Name', exact: true }).fill('Kitchen Keys');
		await page.getByLabel('Type').selectOption('keys');
		await page.keyboard.press('Enter');
		await expect(page.getByRole('heading', { name: 'Saved' })).toBeVisible();
		expect(posts).toEqual([{ id: 'tile:c1d2e3f4a5b6', alias: 'keys:kitchen', name: 'Kitchen Keys' }]);
	});
});
