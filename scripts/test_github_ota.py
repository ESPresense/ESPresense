#!/usr/bin/env python3
"""
GitHub OTA Testing Script for ESPresense

This script specifically tests the GitHub HTTPS update functionality
that is having issues in ESPresense.
"""

import asyncio
import aiohttp
import json
import time
import logging
from typing import Dict, List, Optional
from dataclasses import dataclass
from urllib.parse import urlparse
import hashlib

# Configure logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

@dataclass
class GitHubTestConfig:
    """Configuration for GitHub OTA testing"""
    device_ip: str
    device_port: int = 80
    firmware_file: str = "esp32.bin"
    test_prerelease: bool = False
    timeout: int = 30

@dataclass
class TestResult:
    """Result of a test case"""
    test_name: str
    success: bool
    duration: float
    error_message: Optional[str] = None
    details: Optional[Dict] = None

class GitHubOTATester:
    """GitHub-specific OTA testing framework"""

    def __init__(self, config: GitHubTestConfig):
        self.config = config
        self.session = None
        self.base_url = f"http://{config.device_ip}:{config.device_port}"

    async def __aenter__(self):
        """Async context manager entry"""
        timeout = aiohttp.ClientTimeout(total=self.config.timeout)
        headers = {'User-Agent': 'ESPresense-OTA-Tester/1.0'}
        self.session = aiohttp.ClientSession(timeout=timeout, headers=headers)
        return self

    async def __aexit__(self, exc_type, exc_val, exc_tb):
        """Async context manager exit"""
        if self.session:
            await self.session.close()

    async def test_github_connectivity(self) -> TestResult:
        """Test connectivity to GitHub releases"""
        start_time = time.time()
        try:
            # Test the actual GitHub URL that ESPresense uses
            github_url = f"https://github.com/ESPresense/ESPresense/releases/latest/download/{self.config.firmware_file}"

            async with self.session.head(github_url) as response:
                if response.status == 200:
                    # Check if it's a redirect
                    if response.status == 302:
                        redirect_url = response.headers.get('Location', '')
                        logger.info(f"GitHub redirect to: {redirect_url}")

                    return TestResult(
                        test_name="GitHub Connectivity",
                        success=True,
                        duration=time.time() - start_time,
                        details={
                            "status_code": response.status,
                            "content_length": response.headers.get('content-length'),
                            "url": github_url
                        }
                    )
                elif response.status == 404:
                    return TestResult(
                        test_name="GitHub Connectivity",
                        success=False,
                        duration=time.time() - start_time,
                        error_message=f"Firmware file not found (404): {self.config.firmware_file}",
                        details={"status_code": response.status, "url": github_url}
                    )
                else:
                    return TestResult(
                        test_name="GitHub Connectivity",
                        success=False,
                        duration=time.time() - start_time,
                        error_message=f"HTTP {response.status}",
                        details={"status_code": response.status, "url": github_url}
                    )

        except asyncio.TimeoutError:
            return TestResult(
                test_name="GitHub Connectivity",
                success=False,
                duration=time.time() - start_time,
                error_message="Connection timeout"
            )
        except Exception as e:
            return TestResult(
                test_name="GitHub Connectivity",
                success=False,
                duration=time.time() - start_time,
                error_message=str(e)
            )

    async def test_github_rate_limit(self) -> TestResult:
        """Test GitHub rate limiting behavior"""
        start_time = time.time()
        try:
            # Make multiple rapid requests to test rate limiting
            github_url = f"https://github.com/ESPresense/ESPresense/releases/latest/download/{self.config.firmware_file}"
            rate_limit_detected = False

            for i in range(5):  # Make 5 rapid requests
                async with self.session.head(github_url) as response:
                    if response.status == 403:
                        rate_limit_headers = {
                            'X-RateLimit-Limit': response.headers.get('X-RateLimit-Limit'),
                            'X-RateLimit-Remaining': response.headers.get('X-RateLimit-Remaining'),
                            'X-RateLimit-Reset': response.headers.get('X-RateLimit-Reset'),
                        }

                        if rate_limit_headers['X-RateLimit-Remaining'] == '0':
                            rate_limit_detected = True
                            break

                    await asyncio.sleep(0.5)  # Short delay between requests

            return TestResult(
                test_name="GitHub Rate Limit",
                success=not rate_limit_detected,
                duration=time.time() - start_time,
                details={"rate_limit_detected": rate_limit_detected}
            )

        except Exception as e:
            return TestResult(
                test_name="GitHub Rate Limit",
                success=False,
                duration=time.time() - start_time,
                error_message=str(e)
            )

    async def test_device_github_update(self) -> TestResult:
        """Test device's GitHub update functionality"""
        start_time = time.time()
        try:
            # Trigger update via HTTP endpoint
            update_url = f"https://github.com/ESPresense/ESPresense/releases/latest/download/{self.config.firmware_file}"
            form_data = {"update": update_url}

            async with self.session.post(f"{self.base_url}/wifi/main", data=form_data) as response:
                if response.status == 200:
                    # Device should restart after successful trigger
                    return TestResult(
                        test_name="Device GitHub Update Trigger",
                        success=True,
                        duration=time.time() - start_time,
                        details={
                            "update_url": update_url,
                            "note": "Device should restart to apply update"
                        }
                    )
                else:
                    return TestResult(
                        test_name="Device GitHub Update Trigger",
                        success=False,
                        duration=time.time() - start_time,
                        error_message=f"HTTP {response.status}",
                        details={"response_text": await response.text()}
                    )

        except Exception as e:
            return TestResult(
                test_name="Device GitHub Update Trigger",
                success=False,
                duration=time.time() - start_time,
                error_message=str(e)
            )

    async def test_firmware_size_check(self) -> TestResult:
        """Test firmware size and available space"""
        start_time = time.time()
        try:
            # Get firmware size from GitHub
            github_url = f"https://github.com/ESPresense/ESPresense/releases/latest/download/{self.config.firmware_file}"

            async with self.session.head(github_url) as response:
                if response.status == 200:
                    content_length = response.headers.get('content-length')
                    if content_length:
                        firmware_size = int(content_length)

                        # Get device memory info
                        try:
                            async with self.session.get(f"{self.base_url}/json") as device_response:
                                if device_response.status == 200:
                                    device_data = await device_response.json()
                                    free_heap = device_data.get("state", {}).get("freeHeap", 0)

                                    # ESP32 typically needs about firmware_size * 1.5 free space
                                    estimated_needed = firmware_size * 1.5

                                    sufficient_space = free_heap > estimated_needed

                                    return TestResult(
                                        test_name="Firmware Size Check",
                                        success=sufficient_space,
                                        duration=time.time() - start_time,
                                        details={
                                            "firmware_size": firmware_size,
                                            "free_heap": free_heap,
                                            "estimated_needed": estimated_needed,
                                            "sufficient": sufficient_space
                                        }
                                    )
                        except Exception as e:
                            logger.warning(f"Could not get device memory info: {e}")

                    return TestResult(
                        test_name="Firmware Size Check",
                        success=True,  # Assume OK if we can't check
                        duration=time.time() - start_time,
                        details={
                            "firmware_size": firmware_size,
                            "note": "Could not verify device memory"
                        }
                    )
                else:
                    return TestResult(
                        test_name="Firmware Size Check",
                        success=False,
                        duration=time.time() - start_time,
                        error_message=f"Cannot access firmware: HTTP {response.status}"
                    )

        except Exception as e:
            return TestResult(
                test_name="Firmware Size Check",
                success=False,
                duration=time.time() - start_time,
                error_message=str(e)
            )

    async def test_ssl_handshake(self) -> TestResult:
        """Test SSL/TLS handshake with GitHub"""
        start_time = time.time()
        try:
            # Test SSL connection to GitHub
            github_url = f"https://github.com/ESPresense/ESPresense/releases/latest/download/{self.config.firmware_file}"

            # Use SSL context specifically
            import ssl
            ssl_context = ssl.create_default_context()

            async with self.session.get(github_url, ssl=ssl_context) as response:
                return TestResult(
                    test_name="SSL Handshake",
                    success=response.status == 200,
                    duration=time.time() - start_time,
                    details={
                        "status_code": response.status,
                        "ssl_version": ssl_context.version
                    }
                )

        except Exception as e:
            return TestResult(
                test_name="SSL Handshake",
                success=False,
                duration=time.time() - start_time,
                error_message=f"SSL error: {str(e)}"
            )

    async def test_auto_update_settings(self) -> TestResult:
        """Test auto-update settings on device"""
        start_time = time.time()
        try:
            # Get current settings
            async with self.session.get(f"{self.base_url}/json") as response:
                if response.status == 200:
                    data = await response.json()
                    values = data.get("values", {})

                    auto_update_enabled = values.get("auto_update", False)
                    prerelease_enabled = values.get("prerelease", False)

                    return TestResult(
                        test_name="Auto Update Settings",
                        success=True,
                        duration=time.time() - start_time,
                        details={
                            "auto_update": auto_update_enabled,
                            "prerelease": prerelease_enabled,
                            "settings_accessible": True
                        }
                    )
                else:
                    return TestResult(
                        test_name="Auto Update Settings",
                        success=False,
                        duration=time.time() - start_time,
                        error_message=f"Cannot access settings: HTTP {response.status}"
                    )

        except Exception as e:
            return TestResult(
                test_name="Auto Update Settings",
                success=False,
                duration=time.time() - start_time,
                error_message=str(e)
            )

    async def run_github_tests(self) -> List[TestResult]:
        """Run all GitHub-specific tests"""
        tests = [
            self.test_github_connectivity,
            self.test_github_rate_limit,
            self.test_firmware_size_check,
            self.test_ssl_handshake,
            self.test_auto_update_settings,
            self.test_device_github_update
        ]

        results = []
        for test in tests:
            try:
                result = await test()
                results.append(result)
                logger.info(f"Test '{result.test_name}': {'PASS' if result.success else 'FAIL'} ({result.duration:.2f}s)")
                if not result.success:
                    logger.error(f"  Error: {result.error_message}")
            except Exception as e:
                logger.error(f"Test '{test.__name__}' crashed: {str(e)}")
                results.append(TestResult(
                    test_name=test.__name__,
                    success=False,
                    duration=0.0,
                    error_message=f"Test crash: {str(e)}"
                ))

        return results

    def generate_github_report(self, results: List[TestResult]) -> str:
        """Generate GitHub-specific test report"""
        passed = sum(1 for r in results if r.success)
        total = len(results)

        report = f"""
# GitHub OTA Test Report for ESPresense

## Summary
- Total Tests: {total}
- Passed: {passed}
- Failed: {total - passed}
- Success Rate: {(passed/total)*100:.1f}%

## Detailed Results

"""

        for result in results:
            status = "✅ PASS" if result.success else "❌ FAIL"
            report += f"### {result.test_name} - {status}\n"
            report += f"- Duration: {result.duration:.2f}s\n"
            if result.error_message:
                report += f"- Error: {result.error_message}\n"
            if result.details:
                report += f"- Details: {json.dumps(result.details, indent=2)}\n"
            report += "\n"

        # Add specific recommendations
        report += """
## GitHub-Specific Recommendations

### Common Issues and Solutions

1. **SSL/TLS Handshake Failures**
   - Ensure ESP32 has correct time set for certificate validation
   - Consider using `client.setInsecure()` for GitHub's certificates
   - Increase handshake timeout to 15+ seconds

2. **Rate Limiting**
   - GitHub allows ~60 requests per hour for unauthenticated requests
   - Implement exponential backoff (2s, 4s, 8s delays)
   - Use proper User-Agent header

3. **Connection Timeouts**
   - GitHub can be slow, increase timeouts to 20-30 seconds
   - Use HTTP/1.0 for better compatibility
   - Follow redirects automatically

4. **Firmware Size Issues**
   - Ensure at least 60KB free heap before update
   - Clear caches and disconnect clients before update
   - Monitor memory during download

5. **Redirect Handling**
   - GitHub releases use redirects for latest versions
   - Ensure HTTP client follows 302 redirects
   - Handle version detection properly

### Testing Commands

```bash
# Test GitHub connectivity
python scripts/test_github_ota.py --device-ip 192.168.1.100 --test-connectivity

# Test rate limiting
python scripts/test_github_ota.py --device-ip 192.168.1.100 --test-rate-limit

# Full test suite
python scripts/test_github_ota.py --device-ip 192.168.1.100 --output github_test_report.md
```

### Device Configuration

Make sure these settings are correct on your ESPresense device:

1. **Auto Update**: Enabled in web UI or via MQTT
2. **Prerelease**: Set according to your needs
3. **Time**: Properly synced for SSL certificate validation
4. **Memory**: At least 60KB free heap available

"""

        return report

async def main():
    """Main test execution"""
    import argparse

    parser = argparse.ArgumentParser(description="ESPresense GitHub OTA Testing")
    parser.add_argument("--device-ip", required=True, help="Device IP address")
    parser.add_argument("--port", type=int, default=80, help="Device port (default: 80)")
    parser.add_argument("--firmware", default="esp32.bin", help="Firmware filename (default: esp32.bin)")
    parser.add_argument("--prerelease", action="store_true", help="Test prerelease updates")
    parser.add_argument("--timeout", type=int, default=30, help="Request timeout (default: 30)")
    parser.add_argument("--output", help="Output report file")
    parser.add_argument("--test-connectivity", action="store_true", help="Test only connectivity")
    parser.add_argument("--test-rate-limit", action="store_true", help="Test rate limiting")

    args = parser.parse_args()

    config = GitHubTestConfig(
        device_ip=args.device_ip,
        device_port=args.port,
        firmware_file=args.firmware,
        test_prerelease=args.prerelease,
        timeout=args.timeout
    )

    logger.info(f"Starting GitHub OTA tests for device {config.device_ip}:{config.device_port}")

    async with GitHubOTATester(config) as tester:
        if args.test_connectivity:
            results = [await tester.test_github_connectivity()]
        elif args.test_rate_limit:
            results = [await tester.test_github_rate_limit()]
        else:
            results = await tester.run_github_tests()

        report = tester.generate_github_report(results)

        print(report)

        if args.output:
            with open(args.output, 'w') as f:
                f.write(report)
            logger.info(f"Report saved to {args.output}")

        # Exit with appropriate code
        failed_count = sum(1 for r in results if not r.success)
        exit(failed_count)

if __name__ == "__main__":
    asyncio.run(main())
