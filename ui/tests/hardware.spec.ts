import { test, expect, type Page } from '@playwright/test';

// Mock data for hardware settings
const mockHardwareSettings = {
	values: {
		led_1_type: '0',
		led_1_pin: '2',
		led_1_cnt: '1',
		led_1_cntrl: '1',
		led_2_type: '0',
		led_2_pin: '-1',
		led_2_cnt: '1',
		led_2_cntrl: '0',
		led_3_type: '0',
		led_3_pin: '-1',
		led_3_cnt: '1',
		led_3_cntrl: '0',
		input_count: '1',
		input_1_name: 'Hallway PIR',
		input_1_role: '0',
		input_1_pin: '-1',
		input_1_type: '0',
		input_1_timeout: '5',
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
		led_1_type: '0',
		led_1_pin: '2',
		led_1_cnt: '1',
		led_1_cntrl: '1',
		led_2_pin: '-1',
		input_1_pin: '-1',
		input_1_timeout: '0.5',
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
		await expect(page.locator('select[name="output_count"]')).toBeVisible();

		// Check I2C Settings section exists
		await expect(page.locator('text=I2C Settings')).toBeVisible();
	});

	test('should display current hardware settings values', async ({ page }) => {
		await page.goto('/hardware');

		// Wait for form to load
		await page.waitForSelector('form#hardware');

		// Check LED 1 pin value is loaded
		const led1Pin = page.locator('input[name="led_1_pin"]');
		await expect(led1Pin).toHaveValue('2');

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
		const led1Pin = page.locator('input[name="led_1_pin"]');
		await led1Pin.fill('5');
		await expect(led1Pin).toHaveValue('5');

		// Change LED type dropdown
		const led1Type = page.locator('select[name="led_1_type"]');
		await led1Type.selectOption('2'); // Addressable GRB
		await expect(led1Type).toHaveValue('2');

		// Change input 1 pin
		const inputPin = page.locator('input[name="input_1_pin"]');
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
						values: { ...mockHardwareSettings.values, led_1_pin: '5' }
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
		await page.locator('input[name="led_1_pin"]').fill('5');

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

		const led1Pin = page.locator('input[name="led_1_pin"]');

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

		const led1Control = page.locator('select[name="led_1_cntrl"]');

		// Test all options exist
		await led1Control.selectOption('0'); // MQTT
		await expect(led1Control).toHaveValue('0');

		await led1Control.selectOption('1'); // Status
		await expect(led1Control).toHaveValue('1');

		await led1Control.selectOption('2'); // Motion
		await expect(led1Control).toHaveValue('2');

		await led1Control.selectOption('3'); // Count
		await expect(led1Control).toHaveValue('3');
	});

	test('should handle all LED type options', async ({ page }) => {
		await page.goto('/hardware');
		await page.waitForSelector('form#hardware');

		const led1Type = page.locator('select[name="led_1_type"]');

		// Test all LED type options
		const expectedOptions = [
			{ value: '0', label: 'PWM' },
			{ value: '1', label: 'PWM Inverted' },
			{ value: '2', label: 'Addressable GRB' },
			{ value: '3', label: 'Addressable GRBW' },
			{ value: '4', label: 'Addressable RGB' },
			{ value: '5', label: 'Addressable RGBW' }
		];

		for (const option of expectedOptions) {
			await led1Type.selectOption(option.value);
			await expect(led1Type).toHaveValue(option.value);
		}
	});

	test('should handle input role and pin type options', async ({ page }) => {
		await page.goto('/hardware');
		await page.waitForSelector('form#hardware');

		const role = page.locator('select[name="input_1_role"]');
		for (const v of ['0', '1', '2']) {
			await role.selectOption(v);
			await expect(role).toHaveValue(v);
		}
		const type = page.locator('select[name="input_1_type"]');
		const labels = ['Pullup', 'Pullup Inverted', 'Pulldown', 'Pulldown Inverted', 'Floating', 'Floating Inverted'];
		await expect(type.locator('option:not([hidden])')).toHaveText(labels);
		await type.selectOption('3');
		await expect(type).toHaveValue('3');
	});

	test('pin picker puts the pin type next to the pin', async ({ page }) => {
		await page.goto('/hardware');
		await page.waitForSelector('form#hardware');

		const row = page.locator('input[name="led_1_pin"]').locator('..');
		await expect(row.locator('select[name="led_1_type"]')).toBeVisible();
		await expect(page.locator('input[name="input_1_pin"]').locator('..').locator('select[name="input_1_type"]')).toBeVisible();
	});
});

test.describe('Hardware counted groups', () => {
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

	test('uses firmware defaults when no count is reported', async ({ page }) => {
		await mockSettings(page, {});
		await page.goto('/hardware');
		await page.waitForSelector('form#hardware');

		await expect(page.locator('select[name="led_count"]')).toHaveValue('3');
		await expect(page.locator('h4', { hasText: 'LED 3:' })).toBeVisible();
		await expect(page.locator('select[name="output_count"]')).toHaveValue('0');
		await expect(page.locator('input[name="output_1_pin"]')).toHaveCount(0);
		await expect(page.locator('select[name="input_count"]')).toHaveValue('1');
		await expect(page.locator('input[name="input_1_pin"]')).toBeVisible();
	});

	test('raising a count adds slots that save in the same POST', async ({ page }) => {
		let posted = '';
		await mockSettings(page, {}, (body) => (posted = body));
		await page.goto('/hardware');
		await page.waitForSelector('form#hardware');

		await page.locator('select[name="output_count"]').selectOption('4');
		await expect(page.locator('h4', { hasText: 'Output 4:' })).toBeVisible();
		await page.locator('input[name="output_4_name"]').fill('Plug');
		await page.locator('input[name="output_4_pin"]').fill('5');
		await page.locator('select[name="output_4_type"]').selectOption('1');
		await page.locator('select[name="output_4_state"]').selectOption('2');
		const link = page.locator('select[name="output_4_input"]');
		await expect(link.locator('option', { hasText: '1: Hallway PIR' })).toHaveCount(1);
		await link.selectOption('1');

		await page.locator('button[type="submit"]').click();
		await expect.poll(() => posted).toContain('output_4_pin=5');
		const params = new URLSearchParams(posted);
		expect(params.get('output_count')).toBe('4');
		expect(params.get('output_4_name')).toBe('Plug');
		expect(params.get('output_4_state')).toBe('2');
		expect(params.get('output_4_input')).toBe('1');
		expect(params.get('output_4_type')).toBe('1');
	});

	test('lowering a count hides slots and leaves them out of the POST', async ({ page }) => {
		let posted = '';
		await mockSettings(page, { led_count: 3, input_count: 4, input_4_pin: '9' }, (body) => (posted = body));
		await page.goto('/hardware');
		await page.waitForSelector('form#hardware');

		await expect(page.locator('input[name="input_4_pin"]')).toHaveValue('9');
		await page.locator('select[name="led_count"]').selectOption('1');
		await expect(page.locator('h4', { hasText: 'LED 2:' })).toHaveCount(0);
		await page.locator('select[name="input_count"]').selectOption('2');
		await expect(page.locator('input[name="input_4_pin"]')).toHaveCount(0);

		await page.locator('button[type="submit"]').click();
		await expect.poll(() => posted).toContain('led_count=1');
		expect(posted).not.toContain('led_2_pin');
		expect(posted).not.toContain('input_4_pin');
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
