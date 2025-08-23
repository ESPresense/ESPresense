import { test, expect, type Page } from '@playwright/test';

// Mock data for testing
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

test.describe('Device Re-enrollment Functionality', () => {
  test.beforeEach(async ({ page }) => {
    // Mock the API endpoints
    await page.route('/json/configs', async route => {
      if (route.request().method() === 'GET') {
        await route.fulfill({
          json: { configs: mockDeviceConfigs }
        });
      } else if (route.request().method() === 'POST') {
        const body = await route.request().postDataJSON();
        
        // Simulate the backend behavior: remove existing device with same alias
        const existingIndex = mockDeviceConfigs.findIndex(config => 
          config.alias === body.alias && config.id !== body.id
        );
        
        if (existingIndex !== -1) {
          mockDeviceConfigs.splice(existingIndex, 1);
        }
        
        // Add or update the device
        const deviceIndex = mockDeviceConfigs.findIndex(config => config.id === body.id);
        if (deviceIndex !== -1) {
          mockDeviceConfigs[deviceIndex] = { ...mockDeviceConfigs[deviceIndex], ...body };
        } else {
          mockDeviceConfigs.push({
            id: body.id,
            alias: body.alias || body.id,
            name: body.name,
            'rssi@1m': body['rssi@1m']
          });
        }
        
        await route.fulfill({ json: { success: true } });
      } else if (route.request().method() === 'DELETE') {
        const url = new URL(route.request().url());
        const id = url.searchParams.get('id');
        const index = mockDeviceConfigs.findIndex(config => config.id === id);
        if (index !== -1) {
          mockDeviceConfigs.splice(index, 1);
        }
        await route.fulfill({ json: { success: true } });
      }
    });

    await page.route('/json/events', async route => {
      await route.fulfill({ json: mockEvents });
    });

    await page.goto('/devices');
  });

  test('should display existing devices in the table', async ({ page }) => {
    // Wait for devices to load
    await expect(page.getByText('John Phone')).toBeVisible();
    await expect(page.getByText('Mary Watch')).toBeVisible();
    await expect(page.getByText('phone:john-phone')).toBeVisible();
    await expect(page.getByText('watch:mary-watch')).toBeVisible();
  });

  test('should open enrollment modal when Enroll button is clicked', async ({ page }) => {
    await page.getByRole('button', { name: 'Enroll' }).click();
    
    await expect(page.getByText('Enroll Device - Living Room')).toBeVisible();
    await expect(page.getByText('Select existing ID')).toBeVisible();
    await expect(page.getByPlaceholder('Enter name')).toBeVisible();
  });

  test('should populate existing device dropdown with aliases', async ({ page }) => {
    await page.getByRole('button', { name: 'Enroll' }).click();
    
    const dropdown = page.locator('select').first();
    
    // Check that existing aliases are in the dropdown
    const options = await dropdown.locator('option').allTextContents();
    expect(options).toContain('phone:john-phone');
    expect(options).toContain('watch:mary-watch');
  });

  test('should populate name and ID when existing device is selected', async ({ page }) => {
    await page.getByRole('button', { name: 'Enroll' }).click();
    
    // Select existing device
    const dropdown = page.locator('select').first();
    await dropdown.selectOption('phone:john-phone');
    
    // Check that name and ID fields are populated
    const nameInput = page.getByPlaceholder('Enter name');
    const idInput = page.getByPlaceholder(/Enter custom ID/);
    
    await expect(nameInput).toHaveValue('John Phone');
    await expect(idInput).toHaveValue('phone:john-phone');
  });

  test('should allow enrolling a new device under existing alias', async ({ page }) => {
    await page.getByRole('button', { name: 'Enroll' }).click();
    
    // Select existing device alias
    await page.locator('select').first().selectOption('phone:john-phone');
    
    // Modify name to simulate new device
    const nameInput = page.getByPlaceholder('Enter name');
    await nameInput.clear();
    await nameInput.fill('John New Phone');
    
    // Keep the existing alias
    const idInput = page.getByPlaceholder(/Enter custom ID/);
    await expect(idInput).toHaveValue('phone:john-phone');
    
    // Mock enrollment process
    await page.route('/json/enroll', async route => {
      // Update mock state to simulate enrollment
      mockEvents.state.enrolling = true;
      mockEvents.state.remain = 30;
      await route.fulfill({ json: { success: true } });
    });
    
    await page.getByRole('button', { name: 'Enroll' }).click();
    
    // Should show enrollment in progress
    await expect(page.getByText(/Time remaining/)).toBeVisible();
  });

  test('should generate appropriate ID based on device type and name', async ({ page }) => {
    await page.getByRole('button', { name: 'Enroll' }).click();
    
    // Select device type
    const typeDropdown = page.locator('select').nth(1);
    await typeDropdown.selectOption('phone');
    
    // Enter name
    const nameInput = page.getByPlaceholder('Enter name');
    await nameInput.fill('Test Device');
    
    // Check that ID is auto-generated with type prefix
    const idInput = page.getByPlaceholder(/Enter custom ID/);
    await expect(idInput).toHaveAttribute('placeholder', expect.stringContaining('phone:test-device'));
  });

  test('should handle device editing modal', async ({ page }) => {
    // Click on a device row to open edit modal
    await page.getByText('John Phone').click();
    
    await expect(page.getByText('Edit Device')).toBeVisible();
    
    // Check that form is populated
    const aliasInput = page.getByLabel('Alias');
    await expect(aliasInput).toHaveValue('phone:john-phone');
    
    const nameInput = page.getByLabel('Name');
    await expect(nameInput).toHaveValue('John Phone');
    
    const rssiInput = page.getByLabel('RSSI at 1m (dBm)');
    await expect(rssiInput).toHaveValue('-50');
  });

  test('should allow saving edited device', async ({ page }) => {
    // Click on a device row to open edit modal
    await page.getByText('John Phone').click();
    
    // Edit the name
    const nameInput = page.getByLabel('Name');
    await nameInput.clear();
    await nameInput.fill('John Updated Phone');
    
    // Save changes
    await page.getByRole('button', { name: 'Save' }).click();
    
    // Modal should close
    await expect(page.getByText('Edit Device')).not.toBeVisible();
  });

  test('should allow deleting device with confirmation', async ({ page }) => {
    // Click on a device row to open edit modal
    await page.getByText('John Phone').click();
    
    // Mock the confirmation dialog
    page.on('dialog', async dialog => {
      expect(dialog.message()).toContain('Are you sure you want to delete');
      await dialog.accept();
    });
    
    // Click delete button
    await page.getByRole('button', { name: 'Delete' }).click();
    
    // Modal should close
    await expect(page.getByText('Edit Device')).not.toBeVisible();
  });

  test('should handle enrollment state changes', async ({ page }) => {
    // Mock enrollment starting
    await page.route('/json/enroll', async route => {
      mockEvents.state.enrolling = true;
      mockEvents.state.remain = 30;
      await route.fulfill({ json: { success: true } });
    });
    
    // Mock events endpoint to return enrolling state
    await page.route('/json/events', async route => {
      await route.fulfill({ json: mockEvents });
    });
    
    await page.getByRole('button', { name: 'Enroll' }).click();
    
    // Enter device name
    await page.getByPlaceholder('Enter name').fill('Test Device');
    
    await page.getByRole('button', { name: 'Enroll' }).click();
    
    // Should show enrollment instructions
    await expect(page.getByText(/navigate to your device's Bluetooth settings/)).toBeVisible();
    await expect(page.getByText(/Time remaining/)).toBeVisible();
  });

  test('should show enrollment completion message', async ({ page }) => {
    // Mock enrollment completed state
    mockEvents.state.enrolling = false;
    mockEvents.state.enrolledId = 'phone:test-device-123';
    
    await page.route('/json/events', async route => {
      await route.fulfill({ json: mockEvents });
    });
    
    // Trigger modal opening (simulates enrollment completion)
    await page.evaluate(() => {
      window.dispatchEvent(new CustomEvent('enrollment-complete'));
    });
    
    await page.getByRole('button', { name: 'Enroll' }).click();
    
    // Should show completion message
    await expect(page.getByText(/The device has been enrolled/)).toBeVisible();
    await expect(page.getByText('phone:test-device-123')).toBeVisible();
  });
});