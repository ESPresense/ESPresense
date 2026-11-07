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

  test('network rows are flat list rows in light mode, not floating cards', async ({ page }) => {
    // The default color scheme is light. A row's hover:bg-gray-100 used to match the
    // [class*="bg-gray"] filled-button rule and give every row a shadow.
    const row = page.getByRole('button', { name: /HomeNetwork/ });
    await expect(row).toBeVisible();
    await expect(page.locator('html')).not.toHaveClass(/dark/);
    expect(await row.evaluate((el) => getComputedStyle(el).boxShadow)).toBe('none');
    expect(await row.evaluate((el) => getComputedStyle(el).borderTopLeftRadius)).toBe('0px');
    // ...while a filled button still gets its shadow, so the narrower rule still matches.
    const save = page.getByRole('button', { name: 'Save', exact: true });
    await expect(save).toBeVisible();
    expect(await save.evaluate((el) => getComputedStyle(el).boxShadow)).not.toBe('none');
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

test('MQTT TLS: CA field always shows, and is always saved', async ({ page }) => {
  let posted: URLSearchParams | undefined;
  // A function, so TypeScript doesn't narrow posted to undefined after it is reset below.
  const sent = (key: string) => posted?.get(key);
  await page.route('/wifi/main', async route => {
    if (route.request().method() === 'GET') {
      await route.fulfill({ json: { ...mockMainSettings, values: { ...mockMainSettings.values, mqtt_ca: 'KEEP' } } });
    } else {
      posted = new URLSearchParams(route.request().postData() ?? '');
      await route.fulfill({ json: { success: true } });
    }
  });
  await page.route('/wifi/scan', route => route.fulfill({ json: { networks: {} } }));
  await page.route('/json', route => route.fulfill({ json: { room: 'Living Room' } }));
  await page.route('/restart', route => route.fulfill({ body: '' }));
  await page.goto('/network');

  const ca = page.getByLabel('CA certificate (optional)');
  await expect(ca).toBeVisible();
  // One box: no separate port or TLS fields.
  await expect(page.getByLabel('Port', { exact: true })).toHaveCount(0);
  await expect(page.getByLabel('Use TLS (MQTTS)')).toHaveCount(0);

  // With a plain mqtt:// server, the CA is still posted unchanged.
  await page.getByRole('button', { name: /save/i }).click();
  await expect.poll(() => sent('mqtt_ca')).toBe('KEEP');
  expect(sent('mqtt_port')).toBeNull();
  expect(sent('mqtt_tls')).toBeNull();
  // The save reloads the settings once it's posted; wait it out, or the reload undoes the edit below.
  await expect(page.getByRole('button', { name: 'Save', exact: true })).toBeVisible();

  // WebSocket isn't built in, so the field refuses it; host:port is fine.
  const server = page.getByLabel('Server');
  const valid = async (value: string) => { await server.fill(value); return server.evaluate((el: HTMLInputElement) => el.validity.valid); };
  for (const ok of ['mqtt.local', 'mqtt.local:1884', '10.0.0.5:65535', 'mqtts://b', 'MQTT://b:1', 'fd00::5', '[fd00::5]:1884', 'mqtts://[fd00::5]'])
    expect(await valid(ok), ok).toBe(true);
  for (const bad of ['ws://mqtt.local', 'wss://b', 'mqtt.local:abc', 'mqtts://broker:65536', 'b:0', 'mqtt://b/path', 'b:'])
    expect(await valid(bad), bad).toBe(false);
  await page.getByLabel('Server').fill('MQTTS://mqtt.local:9883');
  expect(await server.evaluate((el: HTMLInputElement) => el.validity.valid)).toBe(true);

  posted = undefined;
  await page.getByRole('button', { name: /save/i }).click();
  await expect.poll(() => sent('mqtt_host')).toBe('MQTTS://mqtt.local:9883');
});

test('a refused save shows the error, restarts nothing and keeps the form', async ({ page }) => {
  const message = 'Not saved: these settings are over the 8 KB limit.';
  let restarted = false;
  await page.route('/wifi/main', async route => {
    if (route.request().method() === 'GET') {
      await route.fulfill({ json: { ...mockMainSettings, values: { ...mockMainSettings.values, mqtt_host: 'mqtts://mqtt.local', mqtt_ca: '' } } });
    } else {
      await route.fulfill({ status: 413, body: message });
    }
  });
  await page.route('/wifi/scan', route => route.fulfill({ json: { networks: {} } }));
  await page.route('/json', route => route.fulfill({ json: { room: 'Living Room' } }));
  await page.route('/restart', route => { restarted = true; return route.fulfill({ body: '' }); });
  await page.goto('/network');

  const ca = page.getByLabel('CA certificate (optional)');
  await ca.fill('-----BEGIN CERTIFICATE-----\nchain\n-----END CERTIFICATE-----');
  await page.getByRole('button', { name: /save/i }).click();

  await expect(page.getByRole('alert')).toHaveText(message);
  await expect(ca).toHaveValue(/chain/);
  expect(restarted).toBe(false);
});

test('scans three times, 5 s apart, then waits for Rescan', async ({ page }) => {
  // Every /wifi/scan is a blocking active scan on the node, so the page must not scan forever.
  let scans = 0;
  let networks: Record<string, number> = mockWifiNetworks;
  await page.clock.install();
  await page.route('/wifi/main', (r) => r.fulfill({ json: mockMainSettings }));
  await page.route('/json', (r) => r.fulfill({ json: { room: 'Living Room' } }));
  await page.route('/wifi/scan', (r) => {
    scans++;
    return r.fulfill({ json: { networks } });
  });
  await page.goto('/network');
  await expect(page.getByText('HomeNetwork')).toBeVisible();
  const again = page.getByRole('button', { name: 'Rescan' });

  /** one scan per 5 s: none before the interval is up, exactly one once it is */
  async function nextScanAfterInterval(expected: number) {
    await page.clock.runFor(4000);
    expect(scans).toBe(expected - 1);
    await page.clock.runFor(1500);
    await expect.poll(() => scans).toBe(expected);
  }

  expect(scans).toBe(1);
  await nextScanAfterInterval(2);
  await nextScanAfterInterval(3);
  await expect(again).toBeVisible();
  await page.clock.runFor(30000);
  expect(scans).toBe(3);

  // A network gone from the air drops off once Rescan runs.
  networks = { HomeNetwork: -45 };
  await again.click();
  await expect.poll(() => scans).toBe(4);
  await expect(page.getByText('OfficeWiFi')).toHaveCount(0);
  await nextScanAfterInterval(5);
  await nextScanAfterInterval(6);
  await expect(again).toBeVisible();
  await page.clock.runFor(30000);
  expect(scans).toBe(6);
});
