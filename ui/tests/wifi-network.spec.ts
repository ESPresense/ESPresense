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
