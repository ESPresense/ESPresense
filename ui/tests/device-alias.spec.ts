import { test, expect, type Page } from '@playwright/test';

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

// Both devices are close on the near step; only device-2 stays close when the device is taken away.
const bothClose = { room: 'Living Room', devices: [
  { id: 'device-1', close: true },
  { id: 'device-2', close: true }
] };
const device1Away = { room: 'Living Room', devices: [
  { id: 'device-2', close: true }
] };

type Reply = { room: string; devices: { id?: string; mac?: string; close: boolean }[] };

// The ID box has no label, and its placeholder turns into the suggested alias once a name is typed.
const idInput = (page: Page) => page.getByRole('dialog', { name: /Enroll Device/ }).getByRole('textbox').nth(1);
const enrollDialog = (page: Page) => page.getByRole('dialog', { name: /Enroll Device/ });

/** Answer POST /json/configs with `status`, and hand back a getter for the posted body */
async function capturePost(page: Page, status = 200) {
  let posted: Record<string, unknown> | null = null;
  await page.route('/json/configs', async route => {
    if (route.request().method() !== 'POST') return route.fallback();
    posted = route.request().postDataJSON();
    await route.fulfill({ status, json: status === 200 ? { success: true } : { error: 'Failed to save config' } });
  });
  return () => posted;
}

test.describe('Device Alias Flow', () => {
  // The reply follows the test's steps, not a call count: the layout's command palette also
  // polls /json/devices, so a count-based mock hands the near reply to whoever asks first.
  let devicesReply: Reply = bothClose;

  /** Run Find: Detect with `near` showing, take it away, Detect with `far` showing */
  async function findDevice(page: Page, near: Reply, far: Reply, expected: string) {
    devicesReply = near;
    await page.getByRole('button', { name: 'Alias', exact: true }).click();
    await page.getByRole('button', { name: 'Detect' }).click();
    await expect(page.getByText('Move the device far away')).toBeVisible();
    devicesReply = far;
    await page.getByRole('button', { name: 'Detect' }).click();
    await expect(page.getByText(`Device identified: ${expected}`)).toBeVisible();
    await page.getByRole('button', { name: 'Use ID' }).click();
  }

  test.beforeEach(async ({ page }) => {
    devicesReply = bothClose;
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

    await page.route('/json/devices?showAll', async route => {
      await route.fulfill({ json: devicesReply });
    });

    await page.goto('/devices');
  });

  test('should alias device using movement detection', async ({ page }) => {
    await findDevice(page, bothClose, device1Away, 'device-1');
    await expect(idInput(page)).toHaveValue('device-1');
    await page.getByPlaceholder('Enter name').fill('Alias Device');
    const posted = await capturePost(page);
    await enrollDialog(page).getByRole('button', { name: 'Enroll' }).click();
    await expect.poll(posted).toEqual({ id: 'device-1', alias: 'device-1', name: 'Alias Device' });
    await expect(enrollDialog(page)).toBeHidden();
  });

  test('saves an aliased device under its existing config key, keeping its calibration', async ({ page }) => {
    await expect(page.getByRole('row', { name: /John Phone/ })).toBeVisible();
    // An aliased fingerprint reports its alias as its id; posting that as the id would make the
    // node delete the original config (it holds the alias) and key a new one by the alias.
    const near: Reply = { room: 'Living Room', devices: [
      { id: 'phone:john-phone', mac: 'aabbccddeeff', close: true },
      { id: 'device-2', close: true }
    ] };
    await findDevice(page, near, device1Away, 'phone:john-phone');
    await expect(idInput(page)).toHaveValue('phone:john-phone');
    await expect(page.getByPlaceholder('Enter name')).toHaveValue('John Phone');
    const posted = await capturePost(page);
    await enrollDialog(page).getByRole('button', { name: 'Enroll' }).click();
    await expect.poll(posted).toEqual({ id: 'phone:existing-phone', alias: 'phone:john-phone', name: 'John Phone', 'rssi@1m': -50 });
  });

  test('keeps the dialog and the form when the node does not save the alias', async ({ page }) => {
    await findDevice(page, bothClose, device1Away, 'device-1');
    await page.getByPlaceholder('Enter name').fill('Alias Device');
    const posted = await capturePost(page, 500);
    await enrollDialog(page).getByRole('button', { name: 'Enroll' }).click();
    await expect.poll(posted).not.toBeNull();
    await expect(enrollDialog(page).getByRole('alert')).toHaveText("The node didn't save it. Try again.");
    await expect(page.getByPlaceholder('Enter name')).toHaveValue('Alias Device');
    await expect(idInput(page)).toHaveValue('device-1');
  });
});
