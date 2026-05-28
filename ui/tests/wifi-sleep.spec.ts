import { test, expect } from '@playwright/test';

const baseSettings = {
    room: 'Living Room',
    defaults: {
        room: 'ESPresense',
        'wifi-ssid': '',
        'wifi-password': '',
        wifi_timeout: 30,
        portal_timeout: 180,
        mqtt_host: 'mqtt.local',
        mqtt_port: 1883,
        wifi_sleep: true,
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
        update: '',
        wifi_sleep: true
    }
};

function makeMocks(page: import('@playwright/test').Page, wifiSleep: boolean, onPost?: (body: string) => void) {
    const settings = {
        ...baseSettings,
        values: { ...baseSettings.values, wifi_sleep: wifiSleep }
    };

    page.route('/wifi/main', async route => {
        if (route.request().method() === 'GET') {
            await route.fulfill({ json: settings });
        } else if (route.request().method() === 'POST') {
            onPost?.(route.request().postData() ?? '');
            await route.fulfill({ status: 200 });
        }
    });
    page.route('/wifi/scan', async route => {
        await route.fulfill({ json: { networks: {} } });
    });
    page.route('/json', async route => {
        await route.fulfill({ json: { room: 'Living Room' } });
    });
    page.route('/restart', async route => {
        await route.fulfill({ status: 200 });
    });
}

test.describe('WiFi sleep checkbox', () => {
    test('shows checked when wifi_sleep is true', async ({ page }) => {
        await makeMocks(page, true);
        await page.goto('/network');
        const checkbox = page.locator('#wifi-sleep');
        await expect(checkbox).toBeVisible();
        await expect(checkbox).toBeChecked();
    });

    test('shows unchecked when wifi_sleep is false', async ({ page }) => {
        await makeMocks(page, false);
        await page.goto('/network');
        const checkbox = page.locator('#wifi-sleep');
        await expect(checkbox).toBeVisible();
        await expect(checkbox).not.toBeChecked();
    });

    test('submits wifi_sleep=1 when checked on save', async ({ page }) => {
        let postedBody = '';
        await makeMocks(page, false, body => { postedBody = body; });
        await page.goto('/network');

        await page.locator('#wifi-sleep').check();
        await page.getByRole('button', { name: 'Save' }).click();

        await expect.poll(() => postedBody).toContain('wifi_sleep=1');
    });

    test('omits wifi_sleep when unchecked on save (backend treats missing as false)', async ({ page }) => {
        let postedBody = '';
        await makeMocks(page, true, body => { postedBody = body; });
        await page.goto('/network');

        await page.locator('#wifi-sleep').uncheck();
        await page.getByRole('button', { name: 'Save' }).click();

        await expect.poll(() => postedBody).not.toContain('wifi_sleep');
    });

    test('toggling updates the label association', async ({ page }) => {
        await makeMocks(page, true);
        await page.goto('/network');

        const label = page.locator('label[for="wifi-sleep"]');
        await expect(label).toContainText('Enable WiFi modem sleep mode');

        // Clicking the label toggles the checkbox
        await label.click();
        await expect(page.locator('#wifi-sleep')).not.toBeChecked();

        await label.click();
        await expect(page.locator('#wifi-sleep')).toBeChecked();
    });
});
