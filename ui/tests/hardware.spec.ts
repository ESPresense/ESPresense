import { test, expect, type Page } from '@playwright/test';

// The <p> fields after a list item's heading (and those of later items), e.g. item(page, 'LED 1:').
const item = (page: Page, heading: string) => page.locator('h4', { hasText: heading }).locator('xpath=following-sibling::p');
const pinRow = (page: Page, heading: string) =>
	item(page, heading).filter({ has: page.getByRole('combobox', { name: 'Pin type' }) }).first();
const pinOf = (page: Page, heading: string) => pinRow(page, heading).locator('input');
const typeOf = (page: Page, heading: string) => pinRow(page, heading).locator('select');

// Mock data for hardware settings
const mockHardwareSettings = {
	values: {
		leds: [
			{ type: 'pwm', pin: 2, control: 'status' },
			{ type: 'pwm', pin: -1, control: 'mqtt' },
			{ type: 'pwm', pin: -1, control: 'mqtt' }
		],
		inputs: [{ name: 'Hallway PIR', role: 'motion', pin: -1, type: 'pullup', timeout: 5 }],
		outputs: [],
		dht11_pin: '-1',
		dht22_pin: '-1',
		dhtTemp_offset: '0',
		dhtHumidity_offset: '0',
		I2C_Bus_1_SDA: '21',
		I2C_Bus_1_SCL: '22',
		I2C_Bus_2_SDA: '-1',
		I2C_Bus_2_SCL: '-1',
		I2CDebug: false,
		AHTX0_I2c_Bus: '1',
		AHTX0_I2c: '0x38',
		BH1750_I2c_Bus: '1',
		BH1750_I2c: '0x23',
		BME280_I2c_Bus: '1',
		BME280_I2c: '0x76',
		BMP180_I2c_Bus: '1',
		BMP180_I2c: '0x77',
		BMP280_I2c_Bus: '1',
		BMP280_I2c: '0x76',
		SHT_I2c_Bus: '1',
		TSL2561_I2c_Bus: '1',
		TSL2561_I2c: '0x39',
		TSL2561_I2c_Gain: 'auto',
		SGP30_I2c_Bus: '1',
		SGP30_I2c: '0x58',
		SCD4x_I2c_Bus: '1',
		SCD4x_I2c: '0x62',
		HX711_sckPin: '-1',
		HX711_doutPin: '-1',
		ds18b20_pin: '-1',
		dsTemp_offset: '0'
	},
	defaults: {
		leds: [{ type: 'pwm', pin: 2, count: 1, control: 'status' }],
		inputs: [],
		outputs: [],
		I2C_Bus_1_SDA: '21',
		I2C_Bus_1_SCL: '22'
	}
};

test.describe('Hardware Settings Page', () => {
	test.beforeEach(async ({ page }) => {
		// Mock the hardware settings endpoint
		await page.route('**/wifi/hardware', async (route) => {
			if (route.request().method() === 'GET') {
				await route.fulfill({
					status: 200,
					contentType: 'application/json',
					body: JSON.stringify(mockHardwareSettings)
				});
			} else {
				await route.fulfill({ status: 200 });
			}
		});

		// Mock the restart endpoint
		await page.route('**/restart', async (route) => {
			await route.abort('failed'); // Simulate connection drop during restart
		});

		// Mock room name endpoint
		await page.route('**/json', async (route) => {
			await route.fulfill({
				status: 200,
				contentType: 'application/json',
				body: JSON.stringify({ room: 'test-room' })
			});
		});
	});

	test('should load hardware page and display form', async ({ page }) => {
		await page.goto('/hardware');

		// Check page loaded
		await expect(page.locator('h2').first()).toBeVisible();

		// Check LED section exists
		await expect(page.locator('text=LED 1:')).toBeVisible();
		await expect(page.locator('text=LED 2:')).toBeVisible();
		await expect(page.locator('text=LED 3:')).toBeVisible();

		// Check Inputs and Outputs sections exist
		await expect(page.locator('h4', { hasText: 'Input 1:' })).toBeVisible();
		await expect(page.getByRole('button', { name: 'Add output' })).toBeVisible();

		// Check I2C Settings section exists
		await expect(page.locator('text=I2C Settings')).toBeVisible();
	});

	test('should display current hardware settings values', async ({ page }) => {
		await page.goto('/hardware');

		// Wait for form to load
		await page.waitForSelector('form#hardware');

		// Check LED 1 pin value is loaded
		await expect(pinOf(page, 'LED 1:')).toHaveValue('2');

		// Check I2C Bus 1 SDA pin
		const i2cSda = page.locator('input[name="I2C_Bus_1_SDA"]');
		await expect(i2cSda).toHaveValue('21');

		// Check I2C Bus 1 SCL pin
		const i2cScl = page.locator('input[name="I2C_Bus_1_SCL"]');
		await expect(i2cScl).toHaveValue('22');
	});

	test('should allow changing form values', async ({ page }) => {
		await page.goto('/hardware');
		await page.waitForSelector('form#hardware');

		// Change LED 1 pin
		const led1Pin = pinOf(page, 'LED 1:');
		await led1Pin.fill('5');
		await expect(led1Pin).toHaveValue('5');

		// Change LED type dropdown; addressable types show a count
		const led1Type = typeOf(page, 'LED 1:');
		await led1Type.selectOption('grb');
		await expect(led1Type).toHaveValue('grb');
		await expect(item(page, 'LED 1:').filter({ hasText: 'Count:' }).first()).toBeVisible();

		// Change input 1 pin
		const inputPin = pinOf(page, 'Input 1:');
		await inputPin.fill('15');
		await expect(inputPin).toHaveValue('15');
	});

	test('should submit form and trigger restart with retries', async ({ page }) => {
		let hardwarePostCalled = false;
		let restartCalled = false;
		let getCallCount = 0;

		// Override routes to track calls
		await page.route('**/wifi/hardware', async (route) => {
			if (route.request().method() === 'GET') {
				getCallCount++;
				// Return updated settings after save
				await route.fulfill({
					status: 200,
					contentType: 'application/json',
					body: JSON.stringify({
						...mockHardwareSettings,
						values: { ...mockHardwareSettings.values, leds: [{ type: 'pwm', pin: 5, control: 'status' }] }
					})
				});
			} else if (route.request().method() === 'POST') {
				hardwarePostCalled = true;
				await route.fulfill({ status: 200 });
			}
		});

		await page.route('**/restart', async (route) => {
			restartCalled = true;
			// Simulate connection failure during restart
			await route.abort('failed');
		});

		await page.goto('/hardware');
		await page.waitForSelector('form#hardware');

		// Change a value
		await pinOf(page, 'LED 1:').fill('5');

		// Submit form
		const initialGetCount = getCallCount;
		await page.locator('button[type="submit"]').click();

		// Wait for save and retry cycle to finish by observing GET retries
		await expect(async () => {
			expect(hardwarePostCalled).toBe(true);
			expect(restartCalled).toBe(true);
			expect(getCallCount).toBeGreaterThan(initialGetCount);
		}).toPass({ timeout: 10000 });
	});

	test('should show saving state on submit button', async ({ page }) => {
		// Gate that holds the POST response until we release it
		let releasePost!: () => void;
		const postGate = new Promise<void>((resolve) => {
			releasePost = resolve;
		});

		await page.route('**/wifi/hardware', async (route) => {
			if (route.request().method() === 'POST') {
				await postGate;
				await route.fulfill({ status: 200 });
			} else {
				await route.fallback();
			}
		});

		await page.goto('/hardware');
		await page.waitForSelector('form#hardware');

		const submitButton = page.locator('button[type="submit"]');

		// Initial state
		await expect(submitButton).toHaveText('Save');

		// Click, then assert "Saving..." while POST is held open
		const clickPromise = submitButton.click();
		await expect(submitButton).toHaveText('Saving...', { timeout: 5000 });

		// Release the POST response
		releasePost();
		await clickPromise;

		// Eventually returns to normal state
		await expect(submitButton).toHaveText('Save', { timeout: 10000 });
	});

	test('should handle API errors gracefully', async ({ page }) => {
		// Mock failing POST request
		await page.route('**/wifi/hardware', async (route) => {
			if (route.request().method() === 'GET') {
				await route.fulfill({
					status: 200,
					contentType: 'application/json',
					body: JSON.stringify(mockHardwareSettings)
				});
			} else if (route.request().method() === 'POST') {
				await route.fulfill({ status: 500, body: 'Internal Server Error' });
			}
		});

		await page.goto('/hardware');
		await page.waitForSelector('form#hardware');

		// Spy on console.error
		const consoleMessages: string[] = [];
		page.on('console', (msg) => {
			if (msg.type() === 'error') {
				consoleMessages.push(msg.text());
			}
		});

		// Try to submit
		await page.locator('button[type="submit"]').click();

		// Button should return to normal state even on error
		await expect(page.locator('button[type="submit"]')).toHaveText('Save', { timeout: 3000 });
	});

	test('should validate pin number ranges', async ({ page }) => {
		await page.goto('/hardware');
		await page.waitForSelector('form#hardware');

		const led1Pin = pinOf(page, 'LED 1:');

		// Test min value
		await led1Pin.fill('-1'); // Valid (disable)
		expect(await led1Pin.inputValue()).toBe('-1');

		// Test max value
		await led1Pin.fill('48'); // Valid max
		expect(await led1Pin.inputValue()).toBe('48');

		// HTML5 validation should prevent > 48
		await led1Pin.fill('49');
		const validationMessage = await led1Pin.evaluate((el: HTMLInputElement) => el.validationMessage);
		expect(validationMessage).toBeTruthy();
	});

	test('should have links to documentation', async ({ page }) => {
		await page.goto('/hardware');

		// Check for documentation links
		const ledLink = page.locator('a[href*="espresense.com/configuration/settings#leds"]');
		await expect(ledLink).toBeVisible();
		await expect(ledLink).toHaveAttribute('target', '_blank');

		const gpioLink = page.locator('a[href*="espresense.com/configuration/settings#gpio-sensors"]');
		await expect(gpioLink).toBeVisible();

		const i2cSettingsLink = page.locator('a[href*="espresense.com/configuration/settings#i2c-settings"]');
		await expect(i2cSettingsLink).toBeVisible();

		const i2cSensorsLink = page.locator('a[href*="espresense.com/configuration/settings#i2c-sensors"]');
		await expect(i2cSensorsLink).toBeVisible();
	});

	test('should retry loading settings after restart', async ({ page }) => {
		let getCallCount = 0;
		let postCalled = false;

		await page.route('**/wifi/hardware', async (route) => {
			if (route.request().method() === 'GET') {
				getCallCount++;

				// First GET is for initial page load - succeed
				if (getCallCount === 1) {
					await route.fulfill({
						status: 200,
						contentType: 'application/json',
						body: JSON.stringify(mockHardwareSettings)
					});
				}
				// After POST, fail first 2 retries, then succeed
				else if (postCalled && getCallCount <= 4) {
					await route.abort('failed');
				} else {
					await route.fulfill({
						status: 200,
						contentType: 'application/json',
						body: JSON.stringify(mockHardwareSettings)
					});
				}
			} else if (route.request().method() === 'POST') {
				postCalled = true;
				await route.fulfill({ status: 200 });
			}
		});

		await page.goto('/hardware');
		await page.waitForSelector('form#hardware');

		const initialGetCount = getCallCount;

		// Submit form
		await page.locator('button[type="submit"]').click();

		// Wait for retries (5 retries × 1s delay = 5s, plus some buffer)
		await expect(async () => {
			expect(getCallCount).toBeGreaterThan(initialGetCount + 2);
		}).toPass({ timeout: 10000 });

		// Should have retried multiple times after the initial load
		expect(getCallCount).toBeGreaterThan(initialGetCount + 2);

		// Form should still be functional after retries succeed
		await expect(page.locator('form#hardware')).toBeVisible();
	});

	test('should handle checkbox inputs', async ({ page }) => {
		await page.goto('/hardware');
		await page.waitForSelector('form#hardware');

		const i2cDebug = page.locator('input[name="I2CDebug"]');

		// Should be unchecked by default (from mock data)
		await expect(i2cDebug).not.toBeChecked();

		// Check it
		await i2cDebug.check();
		await expect(i2cDebug).toBeChecked();

		// Uncheck it
		await i2cDebug.uncheck();
		await expect(i2cDebug).not.toBeChecked();
	});

	test('should handle all LED control options', async ({ page }) => {
		await page.goto('/hardware');
		await page.waitForSelector('form#hardware');

		const control = item(page, 'LED 1:').filter({ hasText: 'LED Control' }).first().locator('select');
		await expect(control.locator('option')).toHaveText(['MQTT', 'Status', 'Motion', 'Count', 'Output']);
		await expect(control).toHaveValue('status');
		await control.selectOption('count');
		await expect(control).toHaveValue('count');
	});

	test('should handle all LED type options', async ({ page }) => {
		await page.goto('/hardware');
		await page.waitForSelector('form#hardware');

		await expect(typeOf(page, 'LED 1:').locator('option')).toHaveText([
			'PWM', 'PWM Inverted', 'Addressable GRB', 'Addressable GRBW', 'Addressable RGB', 'Addressable RGBW'
		]);
	});

	test('should handle input role and pin type options', async ({ page }) => {
		await page.goto('/hardware');
		await page.waitForSelector('form#hardware');

		const role = page.getByLabel('Role');
		await expect(role.locator('option')).toHaveText(['Motion', 'Switch', 'Button']);
		await role.selectOption('button');
		await expect(role).toHaveValue('button');
		const type = typeOf(page, 'Input 1:');
		await expect(type.locator('option')).toHaveText(['Pullup', 'Pullup Inverted', 'Pulldown', 'Pulldown Inverted', 'Floating', 'Floating Inverted']);
	});

	test('pin picker puts the pin type next to the pin', async ({ page }) => {
		await page.goto('/hardware');
		await page.waitForSelector('form#hardware');

		await expect(pinOf(page, 'LED 1:')).toBeVisible();
		await expect(typeOf(page, 'LED 1:')).toBeVisible();
	});
});

test.describe('Hardware lists', () => {
	async function mockSettings(page: Page, values: Record<string, unknown>, onPost?: (body: string) => void) {
		await page.route('**/wifi/hardware', async (route) => {
			if (route.request().method() === 'GET') {
				await route.fulfill({
					status: 200,
					contentType: 'application/json',
					body: JSON.stringify({ ...mockHardwareSettings, values: { ...mockHardwareSettings.values, ...values } })
				});
			} else {
				onPost?.(route.request().postData() ?? '');
				await route.fulfill({ status: 200 });
			}
		});
		await page.route('**/restart', (route) => route.abort('failed'));
		await page.route('**/json', (route) =>
			route.fulfill({ status: 200, contentType: 'application/json', body: JSON.stringify({ room: 'test-room' }) })
		);
	}

	test('lists render one block per item', async ({ page }) => {
		await mockSettings(page, {});
		await page.goto('/hardware');
		await page.waitForSelector('form#hardware');

		await expect(page.locator('h4', { hasText: 'LED 3:' })).toBeVisible();
		await expect(page.locator('h4', { hasText: 'Input 1:' })).toBeVisible();
		await expect(page.locator('h4', { hasText: 'Output 1:' })).toHaveCount(0);
	});

	test('outputs are added and posted as one JSON list', async ({ page }) => {
		let posted = '';
		await mockSettings(page, {}, (body) => (posted = body));
		await page.goto('/hardware');
		await page.waitForSelector('form#hardware');

		await page.getByRole('button', { name: 'Add output' }).click();
		await page.getByRole('button', { name: 'Add output' }).click();
		await expect(page.locator('h4', { hasText: 'Output 2:' })).toBeVisible();
		const out2 = page.locator('h4', { hasText: 'Output 2:' });
		const fields = out2.locator('xpath=following-sibling::p');
		await fields.nth(0).locator('input').fill('Plug');
		await fields.nth(1).locator('input').fill('5');
		await fields.nth(1).locator('select').selectOption('output_inverted');
		await fields.nth(2).locator('select').selectOption('restore');
		const link = fields.nth(3).locator('select');
		await expect(link.locator('option', { hasText: '1: Hallway PIR' })).toHaveCount(1);
		await link.selectOption('1');

		await page.locator('button[type="submit"]').click();
		await expect.poll(() => posted).toContain('outputs=');
		const outputs = JSON.parse(new URLSearchParams(posted).get('outputs')!);
		expect(outputs).toHaveLength(2);
		expect(outputs[1]).toEqual({ name: 'Plug', pin: 5, type: 'output_inverted', power_on: 'restore', input: 1 });
	});

	test('fixed outputs hide power-on state and the linked input', async ({ page }) => {
		await mockSettings(page, { outputs: [{ name: 'LED power', pin: 19, type: 'high' }] });
		await page.goto('/hardware');
		await page.waitForSelector('form#hardware');

		const fields = page.locator('h4', { hasText: 'Output 1:' }).locator('xpath=following-sibling::p');
		await expect(fields.nth(1).locator('select')).toHaveValue('high');
		await expect(page.getByLabel('Power-on state')).toHaveCount(0);
		await fields.nth(1).locator('select').selectOption('output');
		await expect(page.getByLabel('Power-on state')).toHaveCount(1);
	});

	test('removing an item drops it from the POST', async ({ page }) => {
		let posted = '';
		await mockSettings(page, {}, (body) => (posted = body));
		await page.goto('/hardware');
		await page.waitForSelector('form#hardware');

		await page.locator('h4', { hasText: 'Input 1:' }).getByRole('button', { name: 'Remove' }).click();
		await expect(page.locator('h4', { hasText: 'Input 1:' })).toHaveCount(0);
		await page.locator('h4', { hasText: 'LED 3:' }).getByRole('button', { name: 'Remove' }).click();
		await page.locator('h4', { hasText: 'LED 2:' }).getByRole('button', { name: 'Remove' }).click();

		await page.locator('button[type="submit"]').click();
		await expect.poll(() => posted).toContain('leds=');
		expect(JSON.parse(new URLSearchParams(posted).get('leds')!)).toEqual([{ type: 'pwm', pin: 2, control: 'status' }]);
		expect(new URLSearchParams(posted).get('inputs')).toBe('[]');
	});

	test('power monitor posts one JSON object', async ({ page }) => {
		let posted = '';
		await mockSettings(page, {}, (body) => (posted = body));
		await page.goto('/hardware');
		await page.waitForSelector('form#hardware');

		await expect(page.getByLabel('CF pin (power)')).toHaveCount(0);
		await page.getByLabel('Chip (most relay plugs have one)').selectOption('bl0937');
		await page.getByLabel('CF pin (power)').fill('6');
		await page.getByLabel('CF1 pin (V / A)').fill('7');
		await page.getByLabel('SEL pin').fill('10');
		await page.getByLabel("Voltage divider (as in ESPHome's hlw8012)").fill('1517');
		await page.getByLabel('SEL polarity').selectOption('1');

		await page.locator('button[type="submit"]').click();
		await expect.poll(() => posted).toContain('power=');
		expect(JSON.parse(new URLSearchParams(posted).get('power')!)).toEqual({ model: 'bl0937', cf: 6, cf1: 7, sel: 10, voltage_divider: 1517, sel_inverted: true });
	});

	test('an LED can mirror an output', async ({ page }) => {
		let posted = '';
		await mockSettings(page, { outputs: [{ name: 'Relay', pin: 5, type: 'output' }] }, (body) => (posted = body));
		await page.goto('/hardware');
		await page.waitForSelector('form#hardware');

		await item(page, 'LED 1:').filter({ hasText: 'LED Control' }).first().locator('select').selectOption('output');
		await expect(page.getByLabel('Output to show').locator('option')).toHaveText(['1: Relay']);

		await page.locator('button[type="submit"]').click();
		await expect.poll(() => posted).toContain('leds=');
		expect(JSON.parse(new URLSearchParams(posted).get('leds')!)[0]).toMatchObject({ control: 'output' });
	});

	test('LED max brightness defaults by type and posts in the list', async ({ page }) => {
		let posted = '';
		await mockSettings(page, {}, (body) => (posted = body));
		await page.goto('/hardware');
		await page.waitForSelector('form#hardware');

		const max = item(page, 'LED 1:').filter({ hasText: 'Max brightness' }).first().locator('input');
		await expect(max).toHaveValue('255');
		await typeOf(page, 'LED 1:').selectOption('grb');
		await expect(max).toHaveValue('100');
		await max.fill('128');

		await page.locator('button[type="submit"]').click();
		await expect.poll(() => posted).toContain('leds=');
		expect(JSON.parse(new URLSearchParams(posted).get('leds')!)[0]).toMatchObject({ type: 'grb', max_brightness: 128 });
	});
});

test.describe('Hardware Page Integration', () => {
	test('should be accessible from navigation', async ({ page }) => {
		// Mock required endpoints
		await page.route('**/json', async (route) => {
			await route.fulfill({
				status: 200,
				contentType: 'application/json',
				body: JSON.stringify({ room: 'test-room' })
			});
		});

		await page.route('**/wifi/hardware', async (route) => {
			await route.fulfill({
				status: 200,
				contentType: 'application/json',
				body: JSON.stringify(mockHardwareSettings)
			});
		});

		// Go to home page
		await page.goto('/');

		// Look for Hardware link in navigation
		const hardwareLink = page.locator('a[href="/hardware"]');
		await expect(hardwareLink).toBeVisible();

		// Click and verify navigation
		await hardwareLink.click();
		await expect(page).toHaveURL('/hardware');

		// Verify hardware page loaded
		await expect(page.locator('form#hardware')).toBeVisible();
	});
});
