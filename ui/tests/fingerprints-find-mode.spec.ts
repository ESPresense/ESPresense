import { test, expect } from '@playwright/test';

// Mock fingerprints data with various proximity states
const mockFingerprints = [
  {
    mac: 'aa:bb:cc:dd:ee:01',
    id: 'phone:test-device-1',
    name: 'Test Phone 1',
    rssi: -45,
    distance: 1.2,
    close: true,
    vis: true
  },
  {
    mac: 'aa:bb:cc:dd:ee:02', 
    id: 'watch:test-device-2',
    name: 'Test Watch 2',
    rssi: -42,
    distance: 0.8,
    close: true,
    vis: true
  },
  {
    mac: 'aa:bb:cc:dd:ee:03',
    id: 'laptop:test-device-3', 
    name: 'Test Laptop 3',
    rssi: -65,
    distance: 4.5,
    close: false,
    vis: true
  },
  {
    mac: 'aa:bb:cc:dd:ee:04',
    id: 'keys:test-device-4',
    name: 'Test Keys 4', 
    rssi: -38,
    distance: 0.5,
    close: true,
    vis: true
  }
];

let currentMockData = [...mockFingerprints];

test.describe('Fingerprints Find Mode', () => {
  test.beforeEach(async ({ page }) => {
    // Reset mock data
    currentMockData = [...mockFingerprints];

    // Mock the devices endpoint
    await page.route('/json/devices?showAll', async route => {
      await route.fulfill({
        json: {
          room: 'Test Room',
          devices: currentMockData
        }
      });
    });

    // Mock room name endpoint
    await page.route('/json', async route => {
      await route.fulfill({
        json: { room: 'Test Room' }
      });
    });

    // Mock configs endpoint for enrollment modal
    await page.route('/json/configs', async route => {
      await route.fulfill({
        json: {
          room: 'Test Room',
          configs: []
        }
      });
    });

    await page.goto('/fingerprints');
    
    // Wait for table to load
    await expect(page.getByText('Test Phone 1')).toBeVisible();
  });

  test('should display Find Mode toggle', async ({ page }) => {
    await expect(page.getByText('Find Mode')).toBeVisible();
    
    const toggle = page.locator('input[type="checkbox"]');
    await expect(toggle).not.toBeChecked();
  });

  test('should show close devices with yellow highlighting initially', async ({ page }) => {
    // Check that close devices have yellow background (existing functionality)
    const phoneRow = page.getByRole('row', { name: /Test Phone 1/ });
    const watchRow = page.getByRole('row', { name: /Test Watch 2/ });
    const keysRow = page.getByRole('row', { name: /Test Keys 4/ });
    const laptopRow = page.getByRole('row', { name: /Test Laptop 3/ });
    
    await expect(phoneRow).toBeVisible();
    await expect(watchRow).toBeVisible();
    await expect(keysRow).toBeVisible();
    await expect(laptopRow).toBeVisible();
    
    // Close devices should have yellow highlighting (bg-yellow-100 class)
    await expect(phoneRow).toHaveClass(/bg-yellow-100/);
    await expect(watchRow).toHaveClass(/bg-yellow-100/);
    await expect(keysRow).toHaveClass(/bg-yellow-100/);
    
    // Distant device should not have yellow highlighting
    await expect(laptopRow).not.toHaveClass(/bg-yellow-100/);
  });

  test('should show Find Mode status panel when enabled', async ({ page }) => {
    // Enable Find Mode
    await page.getByText('Find Mode').click();
    
    // Should show status panel
    await expect(page.getByText('Find Mode Active')).toBeVisible();
    
    // Should show initial phase
    await expect(page.getByText('📱 Bring Close')).toBeVisible();
  });

  test('should identify close devices as initial candidates', async ({ page }) => {
    // Enable Find Mode
    await page.getByText('Find Mode').click();
    
    // Wait for status panel
    await expect(page.getByText('Find Mode Active')).toBeVisible();
    
    // Should show candidate count
    await expect(page.getByText(/3 candidates found/)).toBeVisible(); // 3 close devices
    
    // Should show "Switch to Move Far" button
    await expect(page.getByRole('button', { name: 'Switch to Move Far' })).toBeVisible();
  });

  test('should show orange highlighting for candidate devices', async ({ page }) => {
    // Enable Find Mode
    await page.getByText('Find Mode').click();
    
    // Wait for candidates to be identified
    await expect(page.getByText(/candidates found/)).toBeVisible();
    
    // Close devices should now have orange highlighting instead of yellow
    const phoneRow = page.getByRole('row', { name: /Test Phone 1/ });
    const watchRow = page.getByRole('row', { name: /Test Watch 2/ });
    const keysRow = page.getByRole('row', { name: /Test Keys 4/ });
    const laptopRow = page.getByRole('row', { name: /Test Laptop 3/ });
    
    await expect(phoneRow).toHaveClass(/bg-orange-200/);
    await expect(watchRow).toHaveClass(/bg-orange-200/);
    await expect(keysRow).toHaveClass(/bg-orange-200/);
    
    // Distant device should still not be highlighted
    await expect(laptopRow).not.toHaveClass(/bg-orange-200/);
    await expect(laptopRow).not.toHaveClass(/bg-yellow-100/);
  });

  test('should switch phases and update instructions', async ({ page }) => {
    // Enable Find Mode
    await page.getByText('Find Mode').click();
    
    // Should start in "Bring Close" phase
    await expect(page.getByText('📱 Bring Close')).toBeVisible();
    await expect(page.getByText(/Bring your target device close, then click/)).toBeVisible();
    
    // Click switch to move far
    await page.getByRole('button', { name: 'Switch to Move Far' }).click();
    
    // Should switch to "Move Far" phase
    await expect(page.getByText('🚶 Move Far')).toBeVisible();
    await expect(page.getByText(/Move your target device further away/)).toBeVisible();
    
    // Button text should change
    await expect(page.getByRole('button', { name: 'Switch to Bring Close' })).toBeVisible();
  });

  test('should filter candidates based on signal changes', async ({ page }) => {
    let updateCount = 0;
    
    // Mock dynamic signal changes
    await page.route('/json/devices?showAll', async route => {
      updateCount++;
      
      if (updateCount <= 3) {
        // Initial state - all close devices are candidates
        await route.fulfill({
          json: {
            room: 'Test Room',
            devices: currentMockData
          }
        });
      } else if (updateCount <= 6) {
        // Simulate "bring close" phase - phone gets stronger signal
        const updatedData = currentMockData.map(device => {
          if (device.mac === 'aa:bb:cc:dd:ee:01') {
            return { ...device, rssi: -40 }; // Stronger signal (less negative)
          }
          return device;
        });
        await route.fulfill({
          json: {
            room: 'Test Room', 
            devices: updatedData
          }
        });
      } else {
        // After phase switch - only responsive device remains candidate
        await route.fulfill({
          json: {
            room: 'Test Room',
            devices: currentMockData.map(device => {
              if (device.mac === 'aa:bb:cc:dd:ee:01') {
                return { ...device, rssi: -40 };
              }
              return device;
            })
          }
        });
      }
    });

    // Enable Find Mode
    await page.getByText('Find Mode').click();
    
    // Wait for initial candidates
    await expect(page.getByText(/3 candidates found/)).toBeVisible();
    
    // Wait a moment for signal history to build
    await page.waitForTimeout(3000);
    
    // Switch to move far phase
    await page.getByRole('button', { name: 'Switch to Move Far' }).click();
    
    // Wait for filtering to occur
    await page.waitForTimeout(2000);
    
    // Should have fewer candidates now (devices that responded to signal change)
    await expect(page.locator('text=/candidates? found/')).toBeVisible();
  });

  test('should show + Add buttons for all devices', async ({ page }) => {
    // Should see + Add buttons in every row
    const addButtons = page.getByRole('button', { name: '+ Add' });
    await expect(addButtons).toHaveCount(4); // One for each device
    
    // Buttons should work with Find Mode off
    await addButtons.first().click();
    await expect(page.getByText('Add Device from Fingerprint')).toBeVisible();
    
    // Close modal
    await page.getByRole('button', { name: 'Cancel' }).click();
    
    // Enable Find Mode - buttons should still be there
    await page.getByText('Find Mode').click();
    await expect(addButtons).toHaveCount(4);
  });

  test('should pre-populate enrollment modal with device data', async ({ page }) => {
    // Click + Add on first device
    const addButtons = page.getByRole('button', { name: '+ Add' });
    await addButtons.first().click();
    
    // Modal should open
    await expect(page.getByText('Add Device from Fingerprint')).toBeVisible();
    
    // Should show fingerprint data
    await expect(page.getByText('MAC: aa:bb:cc:dd:ee:01')).toBeVisible();
    await expect(page.getByText('RSSI: -45 dBm')).toBeVisible();
    await expect(page.getByText('Distance: 1.20 m')).toBeVisible();
    
    // Name field should be pre-populated
    const nameInput = page.getByLabel('Device name');
    await expect(nameInput).toHaveValue('Test Phone 1');
    
    // ID field should be pre-populated with MAC
    const idInput = page.getByLabel('Device ID');
    await expect(idInput).toHaveValue('aa:bb:cc:dd:ee:01');
  });

  test('should show Switch button in top-right when candidates > 1', async ({ page }) => {
    // Enable Find Mode
    await page.getByText('Find Mode').click();
    
    // Wait for candidates
    await expect(page.getByText(/candidates found/)).toBeVisible();
    
    // Should show Switch to Move Far button in top-right (where Reset used to be)
    const switchButton = page.getByRole('button', { name: 'Switch to Move Far' });
    await expect(switchButton).toBeVisible();
    
    // Click switch button
    await switchButton.click();
    
    // Should switch to move far phase
    await expect(page.getByText('🚶 Move Far')).toBeVisible();
    
    // Button text should change
    await expect(page.getByRole('button', { name: 'Switch to Bring Close' })).toBeVisible();
  });

  test('should disable Find Mode and clear state', async ({ page }) => {
    // Enable Find Mode using the toggle input
    const findModeToggle = page.locator('input[type="checkbox"]').first();
    await findModeToggle.click();
    await expect(page.getByText('Find Mode Active')).toBeVisible();
    
    // Disable Find Mode using the same toggle
    await findModeToggle.click();
    
    // Status panel should disappear
    await expect(page.getByText('Find Mode Active')).not.toBeVisible();
    
    // Should return to normal yellow highlighting for close devices
    const phoneRow = page.getByRole('row', { name: /Test Phone 1/ });
    await expect(phoneRow).toHaveClass(/bg-yellow-100/);
    await expect(phoneRow).not.toHaveClass(/bg-orange-200/);
  });
});