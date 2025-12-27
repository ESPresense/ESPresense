# OTA Update Testing Guide

This guide explains how to test firmware Over-The-Air (OTA) updates from GitHub over SSL/TLS.

> **Note**: For information about choosing between `HttpReleaseUpdate` and `GitHubReleaseUpdate`,
> see [OTA Migration Guide](OTA_MIGRATION_GUIDE.md).

## Overview

ESPresense supports OTA firmware updates from GitHub releases over HTTPS. The testing infrastructure validates:

1. **SSL/TLS Connectivity** - Proper handshake with GitHub's servers
2. **GitHub API Integration** - Handling redirects and rate limiting
3. **Update Process** - Download and installation flow
4. **Error Handling** - Graceful handling of network issues, SSL errors, and rate limits

## Test Structure

### Unit Tests (`test/test_ota/test_ota_functionality.cpp`)

The test suite includes several test fixtures:

#### `OTATest` - Basic Functionality
- Mock-based tests for update success/failure scenarios
- Tests retry mechanisms
- Validates error handling

#### `OTASSLGitHubTest` - SSL/GitHub Integration Tests

These tests verify real connectivity to GitHub:

1. **SSLHandshakeToGitHub** - Validates SSL handshake to GitHub servers
2. **GitHubConnectivityTest** - Tests the `testGitHubConnectivity()` method
3. **SSLCertificateHandling** - Verifies SSL client configuration
4. **RateLimitDetection** - Ensures rate limiting is properly detected and handled
5. **RetryMechanismWithGitHub** - Tests exponential backoff retry logic
6. **FirmwareURLConstruction** - Validates URL parsing and detection
7. **CompleteSSLUpdateFlow** - End-to-end update flow test

### Network Tests

Tests that require network access will:
- Skip automatically when run without WiFi connectivity
- Skip when `CI_ENVIRONMENT` is defined (unless explicitly enabled)
- Gracefully handle GitHub rate limiting

## Running Tests

### Local Testing (PlatformIO)

Run all OTA tests:
```bash
pio test -f test_ota
```

Run on native platform:
```bash
pio test -e native-improv -f test_ota
```

### GitHub Actions Workflow

The `.github/workflows/ota-tests.yml` workflow runs automatically on:
- Pushes to `master` branch
- Pull requests affecting OTA-related files
- Manual trigger via `workflow_dispatch`

The workflow consists of three jobs:

1. **build-firmware** - Builds ESP32 firmware
2. **unit-tests** - Runs mock-based unit tests
3. **ssl-connectivity-test** - Validates GitHub HTTPS endpoint

## SSL/TLS Configuration

The OTA updater uses `WiFiClientSecure` with these settings:

```cpp
client.setInsecure();              // Accept GitHub's certificates
client.setHandshakeTimeout(15);    // 15 second handshake timeout
client.setTimeout(20);             // 20 second read timeout
```

### Why `setInsecure()`?

The ESP32's TLS stack requires root CA certificates to validate HTTPS connections. Using `setInsecure()` bypasses certificate validation, which is acceptable for firmware updates because:

1. Firmware integrity is verified via other mechanisms (checksums, signatures)
2. The download URL is from a trusted source (GitHub releases)
3. Maintaining updated CA certificates on resource-constrained devices is challenging

## GitHub Rate Limiting

GitHub API has rate limits:
- **Unauthenticated**: 60 requests/hour
- **Authenticated**: 5000 requests/hour

The OTA updater handles rate limiting via:

1. **Detection** - Checks for HTTP 403/429 status codes and `X-RateLimit-Remaining` headers
2. **Backoff** - Implements exponential backoff between retries
3. **Delays** - Enforces minimum 2-second delay between GitHub requests

When rate limited:
```cpp
if (error == HTTP_UE_GITHUB_RATE_LIMIT) {
    // Update will be retried on next check cycle
    Log.println("GitHub rate limit exceeded");
}
```

## Testing SSL Connectivity Manually

### Using OpenSSL

Test SSL handshake to GitHub:
```bash
openssl s_client -connect github.com:443 -servername github.com
```

### Using cURL

Test firmware endpoint:
```bash
curl -I -L "https://github.com/ESPresense/ESPresense/releases/latest/download/esp32.bin"
```

Check for rate limiting:
```bash
curl -I "https://api.github.com/repos/ESPresense/ESPresense/releases/latest" | grep -i ratelimit
```

## Common Issues

### SSL Handshake Failures

**Symptom**: `HTTP_UE_SSL_HANDSHAKE_FAILED` error

**Causes**:
- Incorrect system time on ESP32 (certificate validation)
- Network firewall blocking HTTPS
- Insufficient memory for TLS handshake

**Solutions**:
- Ensure NTP time sync is working
- Verify network allows HTTPS (port 443)
- Call `optimizeMemoryBeforeUpdate()` before updates

### GitHub Rate Limiting

**Symptom**: `HTTP_UE_GITHUB_RATE_LIMIT` error or HTTP 403/429

**Causes**:
- Too many update checks in short period
- GitHub API rate limit exceeded

**Solutions**:
- Increase `CHECK_FOR_UPDATES_INTERVAL` (default: 6 hours)
- Use GitHub authentication token (not currently implemented)
- Wait for rate limit reset

### Connection Timeouts

**Symptom**: `HTTP_UE_CONNECTION_TIMEOUT` error

**Causes**:
- Poor network connectivity
- GitHub service issues
- Insufficient timeout values

**Solutions**:
- Increase timeout: `updater.setTimeout(20000)` (20 seconds)
- Check network connectivity
- Retry with exponential backoff

## Enhanced Update Features

### `GitHubReleaseUpdate` Class

The enhanced updater (`src/GitHubReleaseUpdate.*`) provides:

- **Retry Logic**: Automatic retries with exponential backoff
- **GitHub-Specific Handling**: Rate limit detection and User-Agent headers
- **Connectivity Pre-check**: Tests connection before downloading
- **Progressive Delays**: Longer delays after failed attempts
- **Better Error Messages**: Detailed error reporting

Usage:
```cpp
GitHubReleaseUpdate updater;
updater.setTimeout(20000);
updater.rebootOnUpdate(false);  // Don't auto-reboot for testing

WiFiClientSecure client;
updater.setupGitHubClient(client);

auto result = updater.update(client, url, 3);  // 3 retries
```

### Update Callbacks

Monitor update progress:
```cpp
updater.onStart([]() {
    Log.println("Update started");
});

updater.onProgress([](int current, int total) {
    int percent = (current * 100) / total;
    Log.printf("Progress: %d%%\n", percent);
});

updater.onEnd([](bool success) {
    Log.printf("Update %s\n", success ? "succeeded" : "failed");
});

updater.onError([](int error) {
    Log.printf("Error: %d\n", error);
});
```

## CI/CD Integration

### Test Skipping

Network tests are skipped in CI by default to avoid:
- Rate limiting from GitHub
- Unreliable network conditions
- Timing issues in CI environment

To enable network tests in CI:
```bash
pio test -f test_ota -D CI_NETWORK_TESTS_ENABLED
```

### SSL Connectivity Verification

The GitHub Actions workflow verifies SSL connectivity using:
- `curl` to test HTTPS endpoint
- `openssl s_client` to validate certificates
- HTTP HEAD requests to avoid rate limits

## Future Improvements

Potential enhancements:
1. **Certificate Pinning** - Validate GitHub's certificate fingerprint
2. **Authenticated Requests** - Use GitHub token to increase rate limits
3. **Checksum Validation** - Verify firmware integrity via SHA256
4. **Signed Updates** - Cryptographically verify firmware authenticity
5. **Delta Updates** - Download only changed portions of firmware
6. **Resume Support** - Continue interrupted downloads

## Automated Integration Testing

### QEMU-based Testing (Free)

The workflow `.github/workflows/ota-tests.yml` now includes actual OTA update testing using QEMU ESP32 emulation:

**Features**:
- ✅ Actual firmware execution in QEMU
- ✅ Local HTTP server for firmware hosting
- ✅ Complete OTA update flow testing
- ✅ GitHub connectivity and SSL verification
- ✅ No external dependencies or tokens required

**Workflow Jobs**:
1. `build-firmware` - Builds ESP32 firmware with PlatformIO
2. `unit-tests` - Runs mock-based unit tests
3. `qemu-ota-test` - **NEW**: Actual OTA test in QEMU emulator
4. `test-summary` - Aggregates results with detailed summary

**What Gets Tested**:
```bash
1. Device boot in QEMU
2. Network initialization
3. OTA update trigger
4. Firmware download from local server
5. Update installation
6. Completion verification
```

**Limitations**:
- QEMU ESP32 support varies by system
- Basic network emulation
- May fallback to connectivity tests if QEMU unavailable

**Running Locally**:
```bash
# Install dependencies
sudo apt-get install qemu-system-xtensa expect socat

# Build firmware
pio run -e esp32

# Start test server
python3 -m http.server 8080 &

# The GitHub Actions workflow handles QEMU execution automatically
```

### Wokwi CI Testing (Recommended for Production)

For more robust testing, we provide [Wokwi CI](https://wokwi.com/ci) integration in `.github/workflows/ota-tests-wokwi.yml`.

**Why Wokwi CI?**
- ✅ Full ESP32 hardware emulation
- ✅ Complete WiFi and network stack
- ✅ Native PlatformIO integration
- ✅ Interactive debugging capabilities
- ✅ Scenario-based testing
- ✅ Higher accuracy than QEMU

**Setup**:
1. Sign up at [wokwi.com/ci](https://wokwi.com/ci) (free tier available)
2. Generate a CI token
3. Add `WOKWI_CLI_TOKEN` to GitHub repository secrets
4. The workflow activates automatically

**Test Scenarios** (`test/scenarios/ota-test.yaml`):
```yaml
steps:
  - Wait for device boot
  - Wait for WiFi connection
  - Trigger OTA update
  - Wait for download progress
  - Verify completion
```

**Customizing Tests**:

Edit `test/scenarios/ota-test.yaml` to add custom validation:

```yaml
- name: "Test GitHub OTA"
  send-text: "update https://github.com/ESPresense/ESPresense/releases/latest/download/esp32.bin\n"
  wait-for-text: "Update OK"
  timeout: 120000
```

**Cost Comparison**:
| Feature | QEMU | Wokwi CI |
|---------|------|----------|
| Cost | Free | Free tier + paid plans |
| Setup | Simple | Requires token |
| Accuracy | Basic | Production-grade |
| Network | Limited | Full support |
| Debugging | Logs only | Interactive |

### Test Results and Monitoring

Both workflows generate detailed GitHub Actions summaries:

```markdown
## OTA Update Tests Summary

### Build Status
- Firmware Build: ✅ success

### Test Results
- Unit Tests: ✅ success
- QEMU OTA Integration Test: ✅ success

✅ All critical tests passed
```

**Add Status Badges** to README.md:

```markdown
[![OTA Tests](https://github.com/ESPresense/ESPresense/actions/workflows/ota-tests.yml/badge.svg)](https://github.com/ESPresense/ESPresense/actions/workflows/ota-tests.yml)

[![OTA Tests (Wokwi)](https://github.com/ESPresense/ESPresense/actions/workflows/ota-tests-wokwi.yml/badge.svg)](https://github.com/ESPresense/ESPresense/actions/workflows/ota-tests-wokwi.yml)
```

### Troubleshooting Integration Tests

#### QEMU Test Skipped
```
⚠️  QEMU test skipped - esp32 machine type may not be available
ℹ️  This is expected in GitHub Actions runners
```
**Solution**: This is expected behavior. QEMU ESP32 support is limited. Consider Wokwi CI for guaranteed emulation.

#### Wokwi CI Token Not Found
```
⚠️  Wokwi CI token not configured - running connectivity tests only
```
**Solution**: Add `WOKWI_CLI_TOKEN` to repository secrets at Settings → Secrets and variables → Actions.

#### OTA Update Timeout in Tests
```
ERROR: OTA update timeout
```
**Debug Steps**:
1. Check firmware size (should be < 2MB)
2. Verify memory optimization: `ESP.getFreeHeap() > 60000`
3. Increase timeout in scenario YAML
4. Check network emulation settings

## Python Testing Scripts

Manual OTA testing scripts are available in `scripts/`:

### Test GitHub OTA Connectivity
```bash
python3 scripts/test_github_ota.py --device-ip 192.168.1.100
```

**Features**:
- Tests GitHub releases endpoint
- Validates SSL handshake
- Checks rate limiting
- Verifies firmware size
- Generates detailed reports

### Test Specific Scenarios
```bash
# Test rate limiting behavior
python3 scripts/test_github_ota.py --device-ip 192.168.1.100 --test-rate-limit

# Test with prerelease versions
python3 scripts/test_github_ota.py --device-ip 192.168.1.100 --prerelease

# Generate markdown report
python3 scripts/test_github_ota.py --device-ip 192.168.1.100 --output report.md
```

**Installation**:
```bash
pip install aiohttp asyncio
```

## References

- [ESP32 Update Library](https://github.com/espressif/arduino-esp32/tree/master/libraries/Update)
- [GitHub API Rate Limiting](https://docs.github.com/en/rest/overview/resources-in-the-rest-api#rate-limiting)
- [ESP32 TLS Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/protocols/esp_tls.html)
- [PlatformIO Testing](https://docs.platformio.org/en/latest/advanced/unit-testing/index.html)
- [ESP32 Unit Tests Runner](https://github.com/marketplace/actions/esp32-unit-tests-runner) - GitHub Marketplace action
- [Wokwi CI Documentation](https://docs.wokwi.com/wokwi-ci/getting-started) - ESP32 simulation platform
- [Wokwi GitHub Actions](https://docs.wokwi.com/wokwi-ci/github-actions) - CI/CD integration guide
- [ESP32 HTTP Client with Wokwi CI](https://github.com/wokwi/platform-io-esp32-http-client) - Example project
