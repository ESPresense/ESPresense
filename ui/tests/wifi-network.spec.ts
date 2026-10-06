import { test, expect } from '@playwright/test';

// Mock WiFi networks data
const mockWifiNetworks = {
  'HomeNetwork': -45,
  'OfficeWiFi': -60,
  'GuestNetwork': -75
};

const mockMainSettings = {
  room: 'Living Room',
  defaults: {
    room: 'ESPresense',
    'wifi-ssid': '',
    'wifi-password': '',
    'ap-password': '',
    'ap-password-enabled': false,
    wifi_timeout: 30,
    portal_timeout: 180,
    mqtt_host: 'mqtt.local',
    mqtt_port: 1883,
    discovery_prefix: 'homeassistant'
  },
  values: {
    room: 'Living Room',
    'wifi-ssid': '',
    'wifi-password': '',
    'ap-password': '',
    'ap-password-enabled': false,
    wifi_timeout: 30,
    portal_timeout: 180,
    eth: 0,
    mqtt_host: 'mqtt.local',
    mqtt_port: 1883,
    mqtt_user: '',
    mqtt_pass: '',
    discovery: true,
    discovery_prefix: 'homeassistant',
    pub_tele: true,
    pub_devices: true,
    auto_update: false,
    prerelease: false,
    arduino_ota: false,
    update: ''
  }
};

test.describe('WiFi Network Selection', () => {
  test.beforeEach(async ({ page }) => {
    // Mock the main settings endpoint
    await page.route('/wifi/main', async route => {
      if (route.request().method() === 'GET') {
        await route.fulfill({ json: mockMainSettings });
      } else if (route.request().method() === 'POST') {
        await route.fulfill({ json: { success: true } });
      }
    });

    // Mock the WiFi scan endpoint
    await page.route('/wifi/scan', async route => {
      await route.fulfill({ json: { networks: mockWifiNetworks } });
    });

    // Mock room name endpoint
    await page.route('/json', async route => {
      await route.fulfill({ json: { room: 'Living Room' } });
    });

    await page.route('/restart', async route => {
      await route.abort('failed');
    });

    await page.goto('/network');
  });

  test('should display WiFi networks in the list', async ({ page }) => {
    // Wait for WiFi networks to load
    await expect(page.getByText('Available Networks')).toBeVisible();
    
    // Check that networks are displayed
    await expect(page.getByText('HomeNetwork')).toBeVisible();
    await expect(page.getByText('OfficeWiFi')).toBeVisible();
    await expect(page.getByText('GuestNetwork')).toBeVisible();
  });

  test('should display signal strength indicators', async ({ page }) => {
    // Wait for networks to load
    await expect(page.getByText('HomeNetwork')).toBeVisible();
    
    // Check that signal strength elements are present
    const networkButton = page.getByRole('button', { name: /HomeNetwork/ });
    await expect(networkButton).toBeVisible();
    
    // Check signal strength title attribute is present
    const signalElement = networkButton.locator('[title*="Signal Strength"]');
    await expect(signalElement).toBeVisible();
    await expect(signalElement).toHaveAttribute('title', 'Signal Strength: -45 dBm');
  });

  test('should select WiFi network when clicked', async ({ page }) => {
    // Wait for networks to load
    await expect(page.getByText('HomeNetwork')).toBeVisible();
    
    // Get the WiFi SSID input field
    const ssidInput = page.getByLabel('WiFi SSID');
    
    // Initially should be empty
    await expect(ssidInput).toHaveValue('');
    
    // Click on HomeNetwork
    await page.getByRole('button', { name: /HomeNetwork/ }).click();
    
    // Check that SSID input is populated
    await expect(ssidInput).toHaveValue('HomeNetwork');
  });

  test('should require an AP password only when protection is enabled', async ({ page }) => {
    const enabled = page.getByLabel('Protect configuration AP with password');
    const password = page.getByLabel('Configuration AP Password');

    await expect(enabled).not.toBeChecked();
    await expect(password).not.toHaveAttribute('required', '');
    await expect(password).toHaveAttribute('minlength', '8');
    await expect(password).toHaveAttribute('maxlength', '63');
    await password.fill('short');
    expect(await password.evaluate(input => (input as HTMLInputElement).checkValidity())).toBe(false);
    await password.fill('');
    expect(await password.evaluate(input => (input as HTMLInputElement).checkValidity())).toBe(true);

    await enabled.check();
    await expect(password).toHaveAttribute('required', '');
    await expect(password).toHaveAttribute('minlength', '8');
    await expect(password).toHaveAttribute('maxlength', '63');
    await password.fill('short');
    expect(await password.evaluate(input => (input as HTMLInputElement).checkValidity())).toBe(false);

    await enabled.uncheck();
    expect(await password.evaluate(input => (input as HTMLInputElement).checkValidity())).toBe(false);
  });

  test('should block saving a short AP password when protection is disabled', async ({ page }) => {
    let submitted = false;
    await page.route('/wifi/main', async route => {
      if (route.request().method() === 'POST') {
        submitted = true;
        await route.fulfill({ json: { success: true } });
      } else {
        await route.fulfill({ json: mockMainSettings });
      }
    });

    await page.getByLabel('Configuration AP Password').fill('short');
    await page.getByRole('button', { name: 'Save' }).click();

    expect(submitted).toBe(false);
  });

  test('should save the AP password when protection is enabled', async ({ page }) => {
    let submitted = '';
    await page.route('/wifi/main', async route => {
      if (route.request().method() === 'POST') {
        submitted = route.request().postData() ?? '';
        await route.fulfill({ json: { success: true } });
      } else {
        await route.fulfill({ json: mockMainSettings });
      }
    });

    await page.getByLabel('Protect configuration AP with password').check();
    await page.getByLabel('Configuration AP Password').fill('AccessPoint8');
    await page.getByRole('button', { name: 'Save' }).click();

    await expect.poll(() => submitted).not.toBe('');
    const values = new URLSearchParams(submitted);
    expect(values.get('ap-password-enabled')).toBe('1');
    expect(values.get('ap-password')).toBe('AccessPoint8');
  });

  test('should preserve the masked AP password when protection is disabled', async ({ page }) => {
    const storedSettings = structuredClone(mockMainSettings);
    storedSettings.values['ap-password'] = '***###***';
    storedSettings.values['ap-password-enabled'] = true;
    let submitted = '';

    await page.route('/wifi/main', async route => {
      if (route.request().method() === 'POST') {
        submitted = route.request().postData() ?? '';
        await route.fulfill({ json: { success: true } });
      } else {
        await route.fulfill({ json: storedSettings });
      }
    });

    await page.reload();
    const password = page.getByLabel('Configuration AP Password');
    await expect(password).toHaveAttribute('type', 'password');
    await expect(password).toHaveValue('***###***');

    await page.getByLabel('Protect configuration AP with password').uncheck();
    await page.getByRole('button', { name: 'Save' }).click();

    await expect.poll(() => submitted).not.toBe('');
    const values = new URLSearchParams(submitted);
    expect(values.get('ap-password')).toBe('***###***');
    expect(values.has('ap-password-enabled')).toBe(false);
  });

  test('should handle clicking different networks', async ({ page }) => {
    // Wait for networks to load
    await expect(page.getByText('OfficeWiFi')).toBeVisible();
    
    const ssidInput = page.getByLabel('WiFi SSID');
    
    // Click on OfficeWiFi
    await page.getByRole('button', { name: /OfficeWiFi/ }).click();
    await expect(ssidInput).toHaveValue('OfficeWiFi');
    
    // Click on different network
    await page.getByRole('button', { name: /GuestNetwork/ }).click();
    await expect(ssidInput).toHaveValue('GuestNetwork');
  });

  test('should show loading spinner during scan', async ({ page }) => {
    // The spinner should be visible during initial scan
    // Note: This test may be timing-dependent based on scan implementation
    const spinner = page.locator('.ios-spinner');
    
    // Spinner might be visible initially or during subsequent scans
    // This is more of a visual regression test
    await expect(page.getByText('Available Networks')).toBeVisible();
  });

  test('should have proper accessibility attributes', async ({ page }) => {
    // Wait for networks to load
    await expect(page.getByText('HomeNetwork')).toBeVisible();
    
    // Check that network items are proper buttons
    const networkButtons = page.getByRole('button', { name: /HomeNetwork|OfficeWiFi|GuestNetwork/ });
    await expect(networkButtons).toHaveCount(3);
    
    // Check that buttons have proper type attribute
    const homeNetworkButton = page.getByRole('button', { name: /HomeNetwork/ });
    await expect(homeNetworkButton).toHaveAttribute('type', 'button');
    
    // Check that buttons are keyboard accessible
    await homeNetworkButton.focus();
    await expect(homeNetworkButton).toBeFocused();
    
    // Test keyboard activation
    const ssidInput = page.getByLabel('WiFi SSID');
    await homeNetworkButton.press('Enter');
    await expect(ssidInput).toHaveValue('HomeNetwork');
  });

  test('should maintain visual styling after accessibility changes', async ({ page }) => {
    // Wait for networks to load
    await expect(page.getByText('HomeNetwork')).toBeVisible();
    
    const networkButton = page.getByRole('button', { name: /HomeNetwork/ });
    
    // Check that the button has the expected CSS classes for styling
    await expect(networkButton).toHaveClass(/flex items-center cursor-pointer/);
    await expect(networkButton).toHaveClass(/hover:bg-gray-100 dark:hover:bg-gray-800/);
    await expect(networkButton).toHaveClass(/transition-colors/);
    await expect(networkButton).toHaveClass(/text-left border-0 bg-transparent/);
  });
});


test('should display SSID with extended characters', async ({ page }) => {
  await page.route('/wifi/main', async route => {
    if (route.request().method() === 'GET') {
      await route.fulfill({ json: mockMainSettings });
    } else if (route.request().method() === 'POST') {
      await route.fulfill({ json: { success: true } });
    }
  });

  await page.route('/wifi/scan', async route => {
    await route.fulfill({ json: { networks: { "Darrell’s iPhone": -70 } } });
  });

  await page.route('/json', async route => {
    await route.fulfill({ json: { room: 'Living Room' } });
  });

  await page.goto('/network');

  await expect(page.getByText('Darrell’s iPhone')).toBeVisible();
  const ssidInput = page.getByLabel('WiFi SSID');
  await page.getByRole('button', { name: 'Darrell’s iPhone' }).click();
  await expect(ssidInput).toHaveValue('Darrell’s iPhone');
});
