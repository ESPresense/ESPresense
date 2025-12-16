# Playwright Tests for ESPresense UI

This directory contains end-to-end tests for the ESPresense web interface using Playwright.

## Test Files

- **`hardware.spec.ts`** - Tests for the Hardware settings page
  - Form loading and display
  - Value binding and updates
  - Save flow with restart and retries
  - Error handling
  - Input validation
  - Documentation links

- **`data-table.spec.ts`** - Tests for the DataTable component
  - Rendering devices and fingerprints
  - Sorting functionality
  - Filtering functionality
  - Row interactions
  - Empty state handling
  - Null/undefined value handling

## Running Tests

### Prerequisites

```bash
# Install dependencies (if not already done)
npm install

# Install Playwright browsers
npx playwright install
```

### Run all tests

```bash
npm run test
```

### Run tests in UI mode (interactive)

```bash
npx playwright test --ui
```

### Run specific test file

```bash
npx playwright test hardware.spec.ts
npx playwright test data-table.spec.ts
```

### Run tests in a specific browser

```bash
npx playwright test --project=chromium
npx playwright test --project=firefox
npx playwright test --project=webkit
```

### Run tests in headed mode (see browser)

```bash
npx playwright test --headed
```

### Debug a specific test

```bash
npx playwright test --debug hardware.spec.ts
```

### Generate test report

```bash
npx playwright show-report
```

## Test Structure

All tests use mocked API endpoints to avoid requiring a live ESP32 device:

- `/wifi/hardware` - Hardware settings endpoint
- `/wifi/extras` - Extra settings endpoint
- `/json` - Room name endpoint
- `/json/devices?showAll` - Devices list
- `/json/configs` - Fingerprints/configs list
- `/restart` - Device restart endpoint
- `/ws` - WebSocket connection

## Writing New Tests

### Basic test structure

```typescript
import { test, expect } from '@playwright/test';

test.describe('Feature Name', () => {
    test.beforeEach(async ({ page }) => {
        // Mock API endpoints
        await page.route('**/api/endpoint', async (route) => {
            await route.fulfill({
                status: 200,
                contentType: 'application/json',
                body: JSON.stringify({ data: 'value' })
            });
        });
    });

    test('should do something', async ({ page }) => {
        await page.goto('/page');

        // Assertions
        await expect(page.locator('selector')).toBeVisible();
    });
});
```

### Mocking API calls

```typescript
// Mock GET request
await page.route('**/api/data', async (route) => {
    if (route.request().method() === 'GET') {
        await route.fulfill({
            status: 200,
            body: JSON.stringify({ result: 'success' })
        });
    }
});

// Mock POST request
await page.route('**/api/save', async (route) => {
    if (route.request().method() === 'POST') {
        await route.fulfill({ status: 200 });
    }
});

// Simulate network error
await page.route('**/api/fail', async (route) => {
    await route.abort('failed');
});
```

### Testing forms

```typescript
// Fill form
await page.locator('input[name="field"]').fill('value');
await page.locator('select[name="dropdown"]').selectOption('option');
await page.locator('input[type="checkbox"]').check();

// Submit
await page.locator('button[type="submit"]').click();

// Verify
await expect(page.locator('text=Success')).toBeVisible();
```

## CI/CD Integration

The tests are configured to run in CI environments:

```yaml
# Example GitHub Actions workflow
- name: Run Playwright tests
  run: npm run test

- name: Upload test results
  if: always()
  uses: actions/upload-artifact@v3
  with:
    name: playwright-report
    path: playwright-report/
```

## Best Practices

1. **Always mock API endpoints** - Tests should not depend on live devices
2. **Use descriptive test names** - "should display hardware settings form"
3. **Wait for elements** - Use `waitForSelector` or `expect().toBeVisible()`
4. **Avoid hardcoded timeouts** - Prefer `waitForTimeout` only when necessary
5. **Test user behavior** - Click, type, navigate like a real user
6. **Test error cases** - Network failures, invalid input, etc.
7. **Keep tests independent** - Each test should work in isolation

## Common Issues

### "Target page, context or browser has been closed"
- The page was closed before the test finished
- Check for navigation or popup handling

### "Timeout exceeded"
- Element not found or API endpoint not mocked
- Increase timeout or check selectors

### "Element is not visible"
- Element may be hidden or not rendered yet
- Use `waitForSelector` or check CSS display properties

## Coverage

Current test coverage:
- ✅ Hardware settings page (form, save, restart, retry logic)
- ✅ DataTable component (sorting, filtering, row clicks)
- ✅ Devices page (using DataTable)
- ✅ Fingerprints page (using DataTable)
- ⏳ Settings page (TODO)
- ⏳ Main navigation (TODO)
- ⏳ Dark mode toggle (TODO)
- ⏳ WebSocket events (TODO)

## Resources

- [Playwright Documentation](https://playwright.dev/)
- [Playwright Best Practices](https://playwright.dev/docs/best-practices)
- [Playwright Debugging](https://playwright.dev/docs/debug)
- [Svelte Testing Tips](https://svelte.dev/docs/testing)
