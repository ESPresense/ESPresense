import { test, expect } from '@playwright/test';

const mockDeviceConfigs = [
  {
    id: 'phone:existing-phone',
    alias: 'phone:john-phone',
    name: 'John Phone',
    'rssi@1m': -50
  },
  {
    id: 'watch:test-watch',
    alias: 'watch:mary-watch',
    name: 'Mary Watch',
    'rssi@1m': -45
  }
];

const mockEvents = {
  room: 'Living Room',
  state: {
    enrolling: false,
    enrolledId: null,
    remain: 0
  }
};

test.describe('Device Alias Flow', () => {
  test.beforeEach(async ({ page }) => {
    await page.addInitScript(() => {
      class MockWebSocket {
        onopen: ((event: Event) => void) | null = null;
        onmessage: ((event: MessageEvent) => void) | null = null;
        onclose: ((event: CloseEvent) => void) | null = null;
        onerror: ((event: Event) => void) | null = null;

        constructor(_url: string) {
          setTimeout(() => {
            if (this.onopen) {
              this.onopen(new Event('open'));
            }
            if (this.onmessage) {
              this.onmessage(new MessageEvent('message', {
                data: JSON.stringify({
                  room: 'Living Room',
                  state: {
                    enrolling: false,
                    enrolledId: null,
                    remain: 0
                  }
                })
              }));
            }
          }, 10);
        }

        send(_data: string) {}
        close() {}
        addEventListener(type: string, listener: EventListener) {
          if (type === 'message') {
            this.onmessage = listener as (event: MessageEvent) => void;
          } else if (type === 'open') {
            this.onopen = listener as (event: Event) => void;
          } else if (type === 'close') {
            this.onclose = listener as (event: CloseEvent) => void;
          } else if (type === 'error') {
            this.onerror = listener as (event: Event) => void;
          }
        }
      }
      (window as any).WebSocket = MockWebSocket;
    });

    await page.route('/json', async route => {
      await route.fulfill({ json: { room: 'Living Room' } });
    });

    await page.route('/json/configs', async route => {
      await route.fulfill({ json: { room: 'Living Room', configs: mockDeviceConfigs } });
    });

    await page.route('/json/events', async route => {
      await route.fulfill({ json: mockEvents });
    });

    const devicesResponses = [
      { room: 'Living Room', devices: [] },
      { room: 'Living Room', devices: [
        { id: 'device-1', close: true },
        { id: 'device-2', close: true }
      ] },
      { room: 'Living Room', devices: [
        { id: 'device-2', close: true }
      ] }
    ];
    let deviceCallCount = 0;
    await page.route('/json/devices?showAll', async route => {
      const response = devicesResponses[Math.min(deviceCallCount, devicesResponses.length - 1)];
      deviceCallCount++;
      await route.fulfill({ json: response });
    });

    await page.goto('/devices');
  });

  test('should alias device using movement detection', async ({ page }) => {
    await page.getByRole('button', { name: 'Alias' }).click();
    await page.getByRole('button', { name: 'Detect' }).click();
    await expect(page.getByText('Move the device far away')).toBeVisible();
    await page.getByRole('button', { name: 'Detect' }).click();
    await expect(page.getByText('Device identified: device-1')).toBeVisible();
    await page.getByRole('button', { name: 'Use ID' }).click();
    await expect(page.getByPlaceholder(/Enter custom ID or leave empty for auto-generated/)).toHaveValue('device-1');
    await page.getByPlaceholder('Enter name').fill('Alias Device');
    let called = false;
    await page.route('/json/alias', async route => {
      called = true;
      await route.fulfill({ json: { success: true } });
    });
    await page.getByRole('button', { name: 'Enroll' }).click();
    expect(called).toBe(true);
  });
});
