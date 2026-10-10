import { test, expect, type Page } from '@playwright/test';

// Board templates (#2529): the Templates page imports a template (preview, then apply) and
// exports this node's board settings.
test.describe('Board template import', () => {
	const plugTemplate = {
		name: 'Athom Smart Plug V3',
		chip: 'esp32c3',
		settings: { led_1_pin: 6, input_1_pin: 3 }
	};
	const plugChanges = [
		{ key: 'led_1_pin', label: 'Pin (-1 to disable)', from: 2, to: 6 },
		{ key: 'input_1_pin', label: 'Pin (-1 to disable)', from: -1, to: 3 }
	];

	/** Records template POSTs; `reply` decides the response per request. */
	async function mockTemplate(
		page: Page,
		reply: (dry: boolean, body: any) => { status: number; body: unknown }
	): Promise<{ dry: boolean; body: any }[]> {
		const calls: { dry: boolean; body: any }[] = [];
		await page.route('**/json/template*', async (route) => {
			const request = route.request();
			if (request.method() !== 'POST') {
				// The page polls GET /json/template to see the node back after a restart.
				return route.fulfill({ status: 200, contentType: 'application/json', body: '{}' });
			}
			const dry = new URL(request.url()).searchParams.has('dry');
			const body = JSON.parse(request.postData() ?? 'null');
			calls.push({ dry, body });
			const r = reply(dry, body);
			await route.fulfill({ status: r.status, contentType: 'application/json', body: JSON.stringify(r.body) });
		});
		return calls;
	}

	test.beforeEach(async ({ page }) => {
		await page.route('**/json', async (route) => {
			await route.fulfill({ status: 200, contentType: 'application/json', body: JSON.stringify({ room: 'test-room' }) });
		});
	});

	test('previews the changes, then applies them', async ({ page }) => {
		const calls = await mockTemplate(page, (dry) => ({ status: 200, body: { changes: plugChanges, restart: !dry } }));
		await page.goto('/templates');

		const apply = page.getByRole('button', { name: 'Apply' });
		await expect(apply).toBeDisabled();

		await page.locator('textarea[name="template"]').fill(JSON.stringify(plugTemplate, null, 2));
		await page.getByRole('button', { name: 'Preview' }).click();

		const table = page.getByTestId('template-changes');
		await expect(table).toBeVisible();
		await expect(table.locator('tbody tr')).toHaveCount(2);
		await expect(table).toContainText('led_1_pin');
		await expect(table).toContainText('input_1_pin');
		expect(calls).toEqual([{ dry: true, body: plugTemplate }]);

		await expect(apply).toBeEnabled();
		await apply.click();
		await expect(page.getByRole('status')).toContainText('Template applied');
		expect(calls[1]).toEqual({ dry: false, body: plugTemplate });
	});

	test('shows the node validation error and does not allow apply', async ({ page }) => {
		await mockTemplate(page, () => ({ status: 400, body: { error: 'template is for esp32c3, this node is esp32' } }));
		await page.goto('/templates');

		await page.locator('textarea[name="template"]').fill(JSON.stringify(plugTemplate));
		await page.getByRole('button', { name: 'Preview' }).click();

		await expect(page.getByRole('alert')).toHaveText('template is for esp32c3, this node is esp32');
		await expect(page.getByRole('button', { name: 'Apply' })).toBeDisabled();
	});

	test('rejects invalid JSON without contacting the node', async ({ page }) => {
		const calls = await mockTemplate(page, () => ({ status: 200, body: { changes: [], restart: false } }));
		await page.goto('/templates');

		await page.locator('textarea[name="template"]').fill('{"chip": "esp32",');
		await page.getByRole('button', { name: 'Preview' }).click();

		await expect(page.getByRole('alert')).toContainText('Not valid JSON');
		expect(calls).toHaveLength(0);
	});

	test('editing the template after preview requires a new preview', async ({ page }) => {
		await mockTemplate(page, () => ({ status: 200, body: { changes: plugChanges, restart: false } }));
		await page.goto('/templates');

		const textarea = page.locator('textarea[name="template"]');
		await textarea.fill(JSON.stringify(plugTemplate));
		await page.getByRole('button', { name: 'Preview' }).click();
		await expect(page.getByRole('button', { name: 'Apply' })).toBeEnabled();

		await textarea.fill(JSON.stringify({ ...plugTemplate, settings: { led_1_pin: 7 } }));
		await expect(page.getByRole('button', { name: 'Apply' })).toBeDisabled();
		await expect(page.getByTestId('template-changes')).toBeHidden();
	});

	test('reports when the board already matches', async ({ page }) => {
		await mockTemplate(page, () => ({ status: 200, body: { changes: [], restart: false } }));
		await page.goto('/templates');

		await page.locator('textarea[name="template"]').fill(JSON.stringify(plugTemplate));
		await page.getByRole('button', { name: 'Preview' }).click();

		await expect(page.getByRole('status')).toHaveText('This board already matches the template.');
		await expect(page.getByRole('button', { name: 'Apply' })).toBeDisabled();
	});

	test('lives on its own page, not the Hardware page', async ({ page }) => {
		await page.route('**/wifi/hardware', async (route) => {
			await route.fulfill({ status: 200, contentType: 'application/json', body: '{"values":{},"defaults":{}}' });
		});
		await page.goto('/hardware');
		await expect(page.locator('form#hardware')).toBeVisible();
		await expect(page.locator('textarea[name="template"]')).toHaveCount(0);

		await page.locator('a[href="/templates"]').click();
		await expect(page).toHaveURL('/templates');
		await expect(page.locator('textarea[name="template"]')).toBeVisible();
	});

	test('accepts settings from any page, e.g. Ethernet type', async ({ page }) => {
		const ethTemplate = { name: 'WT32-ETH01', chip: 'esp32', settings: { eth: 1 } };
		const calls = await mockTemplate(page, () => ({
			status: 200,
			body: { changes: [{ key: 'eth', label: 'Ethernet Type', from: 0, to: 1 }], restart: false }
		}));
		await page.goto('/templates');

		await page.locator('textarea[name="template"]').fill(JSON.stringify(ethTemplate));
		await page.getByRole('button', { name: 'Preview' }).click();

		await expect(page.getByTestId('template-changes')).toContainText('eth');
		expect(calls).toEqual([{ dry: true, body: ethTemplate }]);
	});

	test('exports the node configuration as a template file', async ({ page }) => {
		const exported = { name: 'Office', chip: 'esp32', settings: { led_1_pin: 2, I2CDebug: false } };
		await page.route('**/json/template', async (route) => {
			await route.fulfill({ status: 200, contentType: 'application/json', body: JSON.stringify(exported) });
		});
		await page.goto('/templates');

		const downloadPromise = page.waitForEvent('download');
		await page.getByRole('button', { name: 'Export as template' }).click();
		const download = await downloadPromise;
		expect(download.suggestedFilename()).toBe('Office-template.json');
		const path = await download.path();
		const fs = await import('node:fs/promises');
		expect(JSON.parse(await fs.readFile(path, 'utf8'))).toEqual(exported);
	});
});
