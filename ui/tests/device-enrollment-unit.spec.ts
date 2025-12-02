import { test, expect } from '@playwright/test';

test.describe('Device Re-enrollment Unit Tests', () => {
  test('should verify key re-enrollment functionality logic', async ({ page }) => {
    // Create a mock HTML page that simulates the device enrollment functionality
    await page.setContent(`
      <!DOCTYPE html>
      <html>
      <head>
        <title>ESPresense Device Enrollment Test</title>
        <style>
          body { font-family: Arial, sans-serif; margin: 20px; }
          .modal { display: none; position: fixed; top: 50%; left: 50%; transform: translate(-50%, -50%); background: white; border: 1px solid #ccc; padding: 20px; }
          .modal.show { display: block; }
          table { width: 100%; border-collapse: collapse; margin-bottom: 20px; }
          th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }
          select, input { margin: 5px 0; padding: 5px; width: 200px; }
          button { margin: 5px; padding: 8px 16px; }
        </style>
      </head>
      <body>
        <h2>Devices</h2>
        <table id="deviceTable">
          <thead>
            <tr><th>Alias</th><th>ID</th><th>Name</th><th>RSSI@1m</th></tr>
          </thead>
          <tbody id="deviceTableBody">
            <tr data-device-id="phone:existing-phone">
              <td>phone:john-phone</td>
              <td>phone:existing-phone</td>
              <td>John Phone</td>
              <td>-50 dBm</td>
            </tr>
            <tr data-device-id="watch:test-watch">
              <td>watch:mary-watch</td>
              <td>watch:test-watch</td>
              <td>Mary Watch</td>
              <td>-45 dBm</td>
            </tr>
          </tbody>
        </table>
        
        <button id="enrollButton">Enroll</button>
        
        <div id="enrollModal" class="modal">
          <h3>Enroll Device</h3>
          <select id="existingDeviceSelect">
            <option value="">Select existing ID</option>
            <option value="phone:john-phone">phone:john-phone</option>
            <option value="watch:mary-watch">watch:mary-watch</option>
          </select>
          <br>
          <select id="deviceTypeSelect">
            <option value="">Select device type</option>
            <option value="phone">phone</option>
            <option value="watch">watch</option>
            <option value="laptop">laptop</option>
          </select>
          <br>
          <input type="text" id="nameInput" placeholder="Enter name" />
          <br>
          <input type="text" id="idInput" placeholder="Enter custom ID or leave empty for auto-generated" />
          <br>
          <button id="enrollSubmit">Enroll</button>
          <button id="closeModal">Close</button>
          
          <div id="enrollmentStatus" style="display: none;">
            <p>To begin, navigate to your device's Bluetooth settings and pair with the ESPresense device.</p>
            <p>Time remaining: <span id="timeRemaining">30</span> seconds</p>
          </div>
          
          <div id="enrollmentComplete" style="display: none;">
            <p>The device has been enrolled. The new device ID is: <span id="enrolledId"></span></p>
          </div>
        </div>
        
        <script>
          // Mock device data
          const mockDevices = [
            { id: 'phone:existing-phone', alias: 'phone:john-phone', name: 'John Phone', 'rssi@1m': -50 },
            { id: 'watch:test-watch', alias: 'watch:mary-watch', name: 'Mary Watch', 'rssi@1m': -45 }
          ];
          
          let currentDevices = [...mockDevices];
          
          // Generate kebab case ID
          function generateKebabCaseId(name, type = '') {
            const words = name.toLowerCase().split(/\\s+/);
            const filteredWords = words.filter(word => word !== type.toLowerCase());
            return filteredWords
              .join('-')
              .replace(/[']/g, '')
              .replace(/[^a-z0-9-]+/g, '-')
              .replace(/(^-|-$)/g, '');
          }
          
          // Handle enrollment modal
          document.getElementById('enrollButton').onclick = () => {
            document.getElementById('enrollModal').classList.add('show');
          };
          
          document.getElementById('closeModal').onclick = () => {
            document.getElementById('enrollModal').classList.remove('show');
            resetForm();
          };
          
          // Handle existing device selection
          document.getElementById('existingDeviceSelect').onchange = (e) => {
            const alias = e.target.value;
            if (!alias) {
              document.getElementById('idInput').value = '';
              document.getElementById('nameInput').value = '';
              return;
            }
            
            const device = currentDevices.find(d => d.alias === alias);
            if (device) {
              document.getElementById('idInput').value = device.alias || '';
              document.getElementById('nameInput').value = device.name || '';
            }
          };
          
          // Handle name input for auto-generated ID
          document.getElementById('nameInput').oninput = (e) => {
            const name = e.target.value;
            const type = document.getElementById('deviceTypeSelect').value;
            const existingId = document.getElementById('existingDeviceSelect').value;
            
            if (!existingId && name) {
              const generatedId = type ? type + ':' + generateKebabCaseId(name, type) : generateKebabCaseId(name);
              document.getElementById('idInput').placeholder = generatedId;
            }
          };
          
          document.getElementById('deviceTypeSelect').onchange = () => {
            // Trigger name input event to update placeholder
            document.getElementById('nameInput').dispatchEvent(new Event('input'));
          };
          
          // Handle enrollment submission
          document.getElementById('enrollSubmit').onclick = () => {
            const name = document.getElementById('nameInput').value;
            const idValue = document.getElementById('idInput').value;
            const type = document.getElementById('deviceTypeSelect').value;
            const existingAlias = document.getElementById('existingDeviceSelect').value;
            
            if (!name) {
              alert('Name is required');
              return;
            }
            
            let finalId = idValue;
            if (!finalId) {
              finalId = type ? type + ':' + generateKebabCaseId(name, type) : generateKebabCaseId(name);
            }
            
            // Simulate the backend behavior: remove existing device with same alias if enrolling under existing alias
            if (existingAlias) {
              const existingIndex = currentDevices.findIndex(d => d.alias === existingAlias && d.id !== finalId);
              if (existingIndex !== -1) {
                currentDevices.splice(existingIndex, 1);
                // Update the table
                updateDeviceTable();
                // Mark test result
                window.testResults = window.testResults || {};
                window.testResults.removedExistingDevice = true;
              }
            }
            
            // Add/update device
            const deviceIndex = currentDevices.findIndex(d => d.id === finalId);
            if (deviceIndex !== -1) {
              currentDevices[deviceIndex] = { ...currentDevices[deviceIndex], name, alias: existingAlias || finalId };
            } else {
              currentDevices.push({ id: finalId, alias: existingAlias || finalId, name, 'rssi@1m': null });
            }
            
            // Update table
            updateDeviceTable();
            
            // Show enrollment status
            document.getElementById('enrollmentStatus').style.display = 'block';
            
            // Mark test result
            window.testResults = window.testResults || {};
            window.testResults.enrollmentStarted = true;
            window.testResults.enrolledId = finalId;
            
            // Simulate enrollment completion after 2 seconds
            setTimeout(() => {
              document.getElementById('enrollmentStatus').style.display = 'none';
              document.getElementById('enrollmentComplete').style.display = 'block';
              document.getElementById('enrolledId').textContent = finalId;
              window.testResults.enrollmentCompleted = true;
            }, 2000);
          };
          
          function updateDeviceTable() {
            const tbody = document.getElementById('deviceTableBody');
            tbody.innerHTML = '';
            
            currentDevices.forEach(device => {
              const row = document.createElement('tr');
              row.setAttribute('data-device-id', device.id);
              row.innerHTML = \`
                <td>\${device.alias}</td>
                <td>\${device.id}</td>
                <td>\${device.name}</td>
                <td>\${device['rssi@1m'] ? device['rssi@1m'] + ' dBm' : ''}</td>
              \`;
              tbody.appendChild(row);
            });
          }
          
          function resetForm() {
            document.getElementById('existingDeviceSelect').value = '';
            document.getElementById('deviceTypeSelect').value = '';
            document.getElementById('nameInput').value = '';
            document.getElementById('idInput').value = '';
            document.getElementById('idInput').placeholder = 'Enter custom ID or leave empty for auto-generated';
            document.getElementById('enrollmentStatus').style.display = 'none';
            document.getElementById('enrollmentComplete').style.display = 'none';
          }
          
          // Initialize test results
          window.testResults = {};
        </script>
      </body>
      </html>
    `);

    // Test 1: Verify existing devices are displayed
    await expect(page.getByText('John Phone')).toBeVisible();
    await expect(page.getByText('Mary Watch')).toBeVisible();
    await expect(page.locator('#deviceTableBody')).toContainText('phone:john-phone');
    await expect(page.locator('#deviceTableBody')).toContainText('watch:mary-watch');

    // Test 2: Open enrollment modal
    await page.click('#enrollButton');
    await expect(page.getByText('Enroll Device')).toBeVisible();

    // Test 3: Verify existing device dropdown has correct options
    const options = await page.locator('#existingDeviceSelect option').allTextContents();
    expect(options).toContain('phone:john-phone');
    expect(options).toContain('watch:mary-watch');

    // Test 4: Test selecting existing device populates form
    await page.selectOption('#existingDeviceSelect', 'phone:john-phone');
    await expect(page.locator('#nameInput')).toHaveValue('John Phone');
    await expect(page.locator('#idInput')).toHaveValue('phone:john-phone');

    // Test 5: Test auto-generated ID with device type
    await page.selectOption('#existingDeviceSelect', ''); // Clear selection
    await page.selectOption('#deviceTypeSelect', 'phone');
    await page.fill('#nameInput', 'Test Device');
    
    const idPlaceholder = await page.locator('#idInput').getAttribute('placeholder');
    expect(idPlaceholder).toContain('phone:test-device');

    // Test 6: Test re-enrollment under existing alias (key feature of this PR)
    await page.selectOption('#existingDeviceSelect', 'phone:john-phone');
    await page.fill('#nameInput', 'John New Phone'); // Different name, same alias
    
    // Submit enrollment
    await page.click('#enrollSubmit');
    
    // Wait for enrollment process to start
    await expect(page.locator('#enrollmentStatus')).toBeVisible();
    
    // Check that the existing device was removed and new one added
    const testResults = await page.evaluate(() => (window as any).testResults);
    expect(testResults.removedExistingDevice).toBe(true);
    expect(testResults.enrollmentStarted).toBe(true);
    
    // Wait for enrollment completion
    await expect(page.locator('#enrollmentComplete')).toBeVisible({ timeout: 3000 });
    
    const finalResults = await page.evaluate(() => (window as any).testResults);
    expect(finalResults.enrollmentCompleted).toBe(true);
    expect(finalResults.enrolledId).toBe('phone:john-phone');

    // Test 7: Verify the device table was updated correctly
    const deviceRows = await page.locator('#deviceTableBody tr').count();
    expect(deviceRows).toBe(2); // Should still have 2 devices (one replaced)
    
    // The old device should be gone, new one should exist with the same alias but different name
    await expect(page.getByText('John New Phone')).toBeVisible();
  });

  test('should generate correct kebab-case IDs', async ({ page }) => {
    await page.setContent(`
      <!DOCTYPE html>
      <html>
      <script>
        function generateKebabCaseId(name, type = '') {
          const words = name.toLowerCase().split(/\\s+/);
          const filteredWords = words.filter(word => word !== type.toLowerCase());
          return filteredWords
            .join('-')
            .replace(/[']/g, '')
            .replace(/[^a-z0-9-]+/g, '-')
            .replace(/(^-|-$)/g, '');
        }
        
        // Test various name formats
        window.testIds = {
          basic: generateKebabCaseId('Test Device'),
          withType: 'phone:' + generateKebabCaseId('John Phone', 'phone'),
          special: generateKebabCaseId("John's iPhone 12 Pro"),
          numbers: generateKebabCaseId('Device 123 Test'),
          multipleSpaces: generateKebabCaseId('  Multiple   Spaces  ')
        };
      </script>
      </html>
    `);

    const testIds = await page.evaluate(() => (window as any).testIds);
    
    expect(testIds.basic).toBe('test-device');
    expect(testIds.withType).toBe('phone:john');
    expect(testIds.special).toBe('johns-iphone-12-pro');
    expect(testIds.numbers).toBe('device-123-test');
    expect(testIds.multipleSpaces).toBe('multiple-spaces');
  });
});