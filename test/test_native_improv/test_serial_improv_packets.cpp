#include <array>
#include <cstring>
#include <string>

#include <unity.h>

#include "SerialImprovPackets.h"
#include "../../src/SerialImprovPackets.cpp"

using SerialImprov::Packets::BuildInfoResponse;
using SerialImprov::Packets::BuildRPCResponse;
using SerialImprov::Packets::BuildStateResponse;
using SerialImprov::Packets::DecodeWifiCredentials;

namespace {

uint8_t CalculateChecksum(const SerialImprov::Packets::PacketBuffer& packet) {
    uint16_t sum = 0;
    for (size_t i = 0; i < packet.size - 1; i++) {
        sum += packet.data[i];
    }
    return static_cast<uint8_t>(sum);
}

void assertImprovHeader(const SerialImprov::Packets::PacketBuffer& packet, uint8_t packetType) {
    static const std::array<uint8_t, 6> kHeader = {'I', 'M', 'P', 'R', 'O', 'V'};
    for (size_t i = 0; i < kHeader.size(); i++) {
        TEST_ASSERT_EQUAL_UINT8(kHeader[i], packet.data[i]);
    }
    TEST_ASSERT_EQUAL_UINT8(1, packet.data[6]);  // version
    TEST_ASSERT_EQUAL_UINT8(packetType, packet.data[7]);
}

}  // namespace

void test_state_response_success() {
    auto packet = BuildStateResponse(0x04, false);
    TEST_ASSERT_EQUAL_size_t(11, packet.size);
    assertImprovHeader(packet, 0x01);
    TEST_ASSERT_EQUAL_UINT8(1, packet.data[8]);
    TEST_ASSERT_EQUAL_UINT8(0x04, packet.data[9]);
    TEST_ASSERT_EQUAL_UINT8(CalculateChecksum(packet), packet.data[10]);
}

void test_state_response_error() {
    auto packet = BuildStateResponse(0x02, true);
    TEST_ASSERT_EQUAL_size_t(11, packet.size);
    assertImprovHeader(packet, 0x02);
    TEST_ASSERT_EQUAL_UINT8(1, packet.data[8]);
    TEST_ASSERT_EQUAL_UINT8(0x02, packet.data[9]);
    TEST_ASSERT_EQUAL_UINT8(CalculateChecksum(packet), packet.data[10]);
}

void test_rpc_response_without_url() {
    auto packet = BuildRPCResponse(0x02, nullptr, false);
    TEST_ASSERT_EQUAL_size_t(12, packet.size);
    assertImprovHeader(packet, 0x04);
    TEST_ASSERT_EQUAL_UINT8(2, packet.data[8]);  // payload length
    TEST_ASSERT_EQUAL_UINT8(0x02, packet.data[9]);
    TEST_ASSERT_EQUAL_UINT8(0, packet.data[10]);  // data length
    TEST_ASSERT_EQUAL_UINT8(CalculateChecksum(packet), packet.data[packet.size - 1]);
}

void test_rpc_response_with_url() {
    const char* url = "http://10.0.0.5";
    auto packet = BuildRPCResponse(0x02, url, true);
    const uint8_t urlLength = static_cast<uint8_t>(std::strlen(url));
    TEST_ASSERT_EQUAL_size_t(static_cast<size_t>(13 + urlLength), packet.size);
    assertImprovHeader(packet, 0x04);
    TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(3 + urlLength), packet.data[8]);
    TEST_ASSERT_EQUAL_UINT8(0x02, packet.data[9]);
    TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(1 + urlLength), packet.data[10]);
    TEST_ASSERT_EQUAL_UINT8(urlLength, packet.data[11]);
    TEST_ASSERT_EQUAL_MEMORY(url, packet.data + 12, urlLength);
    TEST_ASSERT_EQUAL_UINT8(CalculateChecksum(packet), packet.data[packet.size - 1]);
}

void test_info_response_payload() {
    auto packet = BuildInfoResponse("ESPresense", "1.2.3", "esp32", "livingroom");
    assertImprovHeader(packet, 0x04);
    const uint8_t payloadLength = packet.data[8];
    const uint8_t dataLength = packet.data[10];
    TEST_ASSERT_EQUAL_UINT8(payloadLength, static_cast<uint8_t>(dataLength + 2));

    const uint8_t firmwareLength = packet.data[11];
    TEST_ASSERT_EQUAL_UINT8(10, firmwareLength);
    TEST_ASSERT_EQUAL_MEMORY("ESPresense", packet.data + 12, firmwareLength);

    size_t cursor = 12 + firmwareLength;
    const uint8_t versionLength = packet.data[cursor];
    cursor += 1;
    TEST_ASSERT_EQUAL_UINT8(5, versionLength);
    TEST_ASSERT_EQUAL_MEMORY("1.2.3", packet.data + cursor, versionLength);
    cursor += versionLength;

    const uint8_t hardwareLength = packet.data[cursor];
    cursor += 1;
    TEST_ASSERT_EQUAL_UINT8(5, hardwareLength);
    TEST_ASSERT_EQUAL_MEMORY("esp32", packet.data + cursor, hardwareLength);
    cursor += hardwareLength;

    const uint8_t roomLength = packet.data[cursor];
    cursor += 1;
    TEST_ASSERT_EQUAL_UINT8(10, roomLength);
    TEST_ASSERT_EQUAL_MEMORY("livingroom", packet.data + cursor, roomLength);

    TEST_ASSERT_EQUAL_UINT8(CalculateChecksum(packet), packet.data[packet.size - 1]);
}

void test_decode_wifi_credentials_success() {
    const char ssid[] = "TestWifi";
    const char password[] = "SuperSecret";

    const uint8_t ssidLen = sizeof(ssid) - 1;
    const uint8_t passLen = sizeof(password) - 1;
    const uint8_t totalLen = static_cast<uint8_t>(ssidLen + passLen + 2);

    uint8_t payload[1 + totalLen] = {};
    payload[0] = totalLen;
    payload[1] = ssidLen;
    std::memcpy(payload + 2, ssid, ssidLen);
    payload[2 + ssidLen] = passLen;
    std::memcpy(payload + 3 + ssidLen, password, passLen);

    std::string decodedSsid;
    std::string decodedPassword;
    bool decoded = DecodeWifiCredentials(payload, payload[0], decodedSsid, decodedPassword);
    TEST_ASSERT_TRUE(decoded);
    TEST_ASSERT_EQUAL_STRING(ssid, decodedSsid.c_str());
    TEST_ASSERT_EQUAL_STRING(password, decodedPassword.c_str());
}

void test_decode_wifi_credentials_rejects_invalid() {
    uint8_t payload[] = {1, 0xFF};
    std::string ssid;
    std::string password;
    TEST_ASSERT_FALSE(DecodeWifiCredentials(payload, payload[0], ssid, password));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_state_response_success);
    RUN_TEST(test_state_response_error);
    RUN_TEST(test_rpc_response_without_url);
    RUN_TEST(test_rpc_response_with_url);
    RUN_TEST(test_info_response_payload);
    RUN_TEST(test_decode_wifi_credentials_success);
    RUN_TEST(test_decode_wifi_credentials_rejects_invalid);
    return UNITY_END();
}
