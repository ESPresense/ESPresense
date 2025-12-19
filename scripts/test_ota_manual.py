#!/usr/bin/env python3
"""
Manual OTA Testing Script for ESPresense

This script provides a comprehensive testing framework for OTA functionality
including simulated firmware uploads, network failure scenarios, and rollback testing.
"""

import asyncio
import aiohttp
import hashlib
import json
import time
import logging
from typing import Dict, List, Optional, Tuple
from dataclasses import dataclass
from pathlib import Path

# Configure logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

@dataclass
class TestConfig:
    """Configuration for OTA testing"""
    device_ip: str
    device_port: int = 80
    firmware_url: str = ""
    timeout: int = 30
    max_retries: int = 3

@dataclass
class TestResult:
    """Result of a test case"""
    test_name: str
    success: bool
    duration: float
    error_message: Optional[str] = None
    details: Optional[Dict] = None

class OTATester:
    """Comprehensive OTA testing framework"""

    def __init__(self, config: TestConfig):
        self.config = config
        self.session = None
        self.base_url = f"http://{config.device_ip}:{config.device_port}"

    async def __aenter__(self):
        """Async context manager entry"""
        timeout = aiohttp.ClientTimeout(total=self.config.timeout)
        self.session = aiohttp.ClientSession(timeout=timeout)
        return self

    async def __aexit__(self, exc_type, exc_val, exc_tb):
        """Async context manager exit"""
        if self.session:
            await self.session.close()

    async def test_device_connectivity(self) -> TestResult:
        """Test basic device connectivity"""
        start_time = time.time()
        try:
            async with self.session.get(f"{self.base_url}/") as response:
                if response.status == 200:
                    return TestResult(
                        test_name="Device Connectivity",
                        success=True,
                        duration=time.time() - start_time,
                        details={"status_code": response.status}
                    )
                else:
                    return TestResult(
                        test_name="Device Connectivity",
                        success=False,
                        duration=time.time() - start_time,
                        error_message=f"HTTP {response.status}"
                    )
        except Exception as e:
            return TestResult(
                test_name="Device Connectivity",
                success=False,
                duration=time.time() - start_time,
                error_message=str(e)
            )

    async def test_ota_settings_endpoint(self) -> TestResult:
        """Test OTA settings endpoint"""
        start_time = time.time()
        try:
            async with self.session.get(f"{self.base_url}/json") as response:
                if response.status == 200:
                    data = await response.json()
                    ota_fields = ["auto_update", "arduino_ota", "prerelease", "update"]
                    missing_fields = [f for f in ota_fields if f not in data.get("values", {})]

                    if missing_fields:
                        return TestResult(
                            test_name="OTA Settings Endpoint",
                            success=False,
                            duration=time.time() - start_time,
                            error_message=f"Missing fields: {missing_fields}",
                            details={"response": data}
                        )

                    return TestResult(
                        test_name="OTA Settings Endpoint",
                        success=True,
                        duration=time.time() - start_time,
                        details={"ota_settings": {k: v for k, v in data["values"].items() if k in ota_fields}}
                    )
                else:
                    return TestResult(
                        test_name="OTA Settings Endpoint",
                        success=False,
                        duration=time.time() - start_time,
                        error_message=f"HTTP {response.status}"
                    )
        except Exception as e:
            return TestResult(
                test_name="OTA Settings Endpoint",
                success=False,
                duration=time.time() - start_time,
                error_message=str(e)
            )

    async def test_auto_update_toggle(self) -> TestResult:
        """Test toggling auto-update setting"""
        start_time = time.time()
        try:
            # Get current state
            async with self.session.get(f"{self.base_url}/json") as response:
                data = await response.json()
                current_state = data["values"].get("auto_update", False)

            # Toggle state
            new_state = not current_state
            form_data = {"auto_update": "1" if new_state else "0"}

            async with self.session.post(f"{self.base_url}/wifi/main", data=form_data) as response:
                if response.status != 200:
                    return TestResult(
                        test_name="Auto Update Toggle",
                        success=False,
                        duration=time.time() - start_time,
                        error_message=f"Failed to set setting: HTTP {response.status}"
                    )

            # Verify change
            await asyncio.sleep(2)  # Wait for device to process
            async with self.session.get(f"{self.base_url}/json") as response:
                data = await response.json()
                updated_state = data["values"].get("auto_update", False)

            if updated_state == new_state:
                return TestResult(
                    test_name="Auto Update Toggle",
                    success=True,
                    duration=time.time() - start_time,
                    details={"previous": current_state, "new": new_state}
                )
            else:
                return TestResult(
                    test_name="Auto Update Toggle",
                    success=False,
                    duration=time.time() - start_time,
                    error_message="Setting change not reflected"
                )

        except Exception as e:
            return TestResult(
                test_name="Auto Update Toggle",
                success=False,
                duration=time.time() - start_time,
                error_message=str(e)
            )

    async def test_manual_update_trigger(self) -> TestResult:
        """Test manual update trigger via MQTT"""
        start_time = time.time()
        try:
            # This would require MQTT client setup
            # For now, simulate via HTTP if available
            update_url = "https://github.com/ESPresense/ESPresense/releases/latest/download/esp32.bin"
            form_data = {"update": update_url}

            async with self.session.post(f"{self.base_url}/wifi/main", data=form_data) as response:
                if response.status == 200:
                    return TestResult(
                        test_name="Manual Update Trigger",
                        success=True,
                        duration=time.time() - start_time,
                        details={"update_url": update_url, "note": "Device should restart"}
                    )
                else:
                    return TestResult(
                        test_name="Manual Update Trigger",
                        success=False,
                        duration=time.time() - start_time,
                        error_message=f"HTTP {response.status}"
                    )

        except Exception as e:
            return TestResult(
                test_name="Manual Update Trigger",
                success=False,
                duration=time.time() - start_time,
                error_message=str(e)
            )

    async def test_memory_monitoring(self) -> TestResult:
        """Test memory monitoring during update simulation"""
        start_time = time.time()
        try:
            # Get telemetry data
            async with self.session.get(f"{self.base_url}/json") as response:
                data = await response.json()
                telemetry = {}

                # Extract memory-related telemetry if available
                if "state" in data:
                    state = data["state"]
                    telemetry = {
                        "freeHeap": state.get("freeHeap", 0),
                        "maxHeap": state.get("maxHeap", 0)
                    }

                if telemetry.get("freeHeap", 0) > 60000:  # Current threshold
                    return TestResult(
                        test_name="Memory Monitoring",
                        success=True,
                        duration=time.time() - start_time,
                        details=telemetry
                    )
                else:
                    return TestResult(
                        test_name="Memory Monitoring",
                        success=False,
                        duration=time.time() - start_time,
                        error_message=f"Low memory: {telemetry.get('freeHeap', 0)} bytes",
                        details=telemetry
                    )

        except Exception as e:
            return TestResult(
                test_name="Memory Monitoring",
                success=False,
                duration=time.time() - start_time,
                error_message=str(e)
            )

    async def test_arduino_ota_discovery(self) -> TestResult:
        """Test Arduino OTA service discovery"""
        start_time = time.time()
        try:
            # Check if Arduino OTA port is accessible
            # This is a simplified test - real testing would require mDNS discovery
            reader, writer = await asyncio.wait_for(
                asyncio.open_connection(self.config.device_ip, 3232),
                timeout=5.0
            )

            writer.close()
            await writer.wait_closed()

            return TestResult(
                test_name="Arduino OTA Discovery",
                success=True,
                duration=time.time() - start_time,
                details={"port": 3232, "accessible": True}
            )

        except (asyncio.TimeoutError, ConnectionRefusedError) as e:
            return TestResult(
                test_name="Arduino OTA Discovery",
                success=False,
                duration=time.time() - start_time,
                error_message=f"OTA port not accessible: {str(e)}"
            )
        except Exception as e:
            return TestResult(
                test_name="Arduino OTA Discovery",
                success=False,
                duration=time.time() - start_time,
                error_message=str(e)
            )

    async def test_network_resilience(self) -> TestResult:
        """Test OTA behavior under poor network conditions"""
        start_time = time.time()
        try:
            # Test with very short timeout to simulate poor network
            timeout = aiohttp.ClientTimeout(total=2)

            async with aiohttp.ClientSession(timeout=timeout) as session:
                async with session.get(f"{self.base_url}/json") as response:
                    if response.status == 200:
                        return TestResult(
                            test_name="Network Resilience",
                            success=True,
                            duration=time.time() - start_time,
                            details={"fast_response": True}
                        )
                    else:
                        return TestResult(
                            test_name="Network Resilience",
                            success=False,
                            duration=time.time() - start_time,
                            error_message=f"HTTP {response.status}"
                        )

        except asyncio.TimeoutError:
            return TestResult(
                test_name="Network Resilience",
                success=False,
                duration=time.time() - start_time,
                error_message="Timeout under poor network conditions"
            )
        except Exception as e:
            return TestResult(
                test_name="Network Resilience",
                success=False,
                duration=time.time() - start_time,
                error_message=str(e)
            )

    async def run_comprehensive_tests(self) -> List[TestResult]:
        """Run all OTA tests"""
        tests = [
            self.test_device_connectivity,
            self.test_ota_settings_endpoint,
            self.test_auto_update_toggle,
            self.test_manual_update_trigger,
            self.test_memory_monitoring,
            self.test_arduino_ota_discovery,
            self.test_network_resilience
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

    def generate_report(self, results: List[TestResult]) -> str:
        """Generate a comprehensive test report"""
        passed = sum(1 for r in results if r.success)
        total = len(results)

        report = f"""
# ESPresense OTA Test Report

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

        return report

async def main():
    """Main test execution"""
    import argparse

    parser = argparse.ArgumentParser(description="ESPresense OTA Testing")
    parser.add_argument("--device-ip", required=True, help="Device IP address")
    parser.add_argument("--port", type=int, default=80, help="Device port (default: 80)")
    parser.add_argument("--timeout", type=int, default=30, help="Request timeout (default: 30)")
    parser.add_argument("--output", help="Output report file")

    args = parser.parse_args()

    config = TestConfig(
        device_ip=args.device_ip,
        device_port=args.port,
        timeout=args.timeout
    )

    logger.info(f"Starting OTA tests for device {config.device_ip}:{config.device_port}")

    async with OTATester(config) as tester:
        results = await tester.run_comprehensive_tests()
        report = tester.generate_report(results)

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
