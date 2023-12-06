#include "ChannelInfo.h"
#include <Arduino.h>
#include <atomic>
#include "esp_attr.h"

#define BLE_RXDESC_OFFSET 0x1000

extern "C" void *r_emi_get_mem_addr_by_offset(size_t offset);
extern "C" void __real_r_lld_scan_process_pkt_rx_adv_rep(int scan_index, unsigned long param_2,int rxdesc_index, void *param_4);
extern "C" void __real_r_ke_msg_send(void *msg);

struct BleRxDescriptor {
    uint16_t a[3];
    uint8_t rssi;
    uint8_t channel;
    uint16_t b[6];
};

#define BLE_CHANNEL_QUEUE_SIZE 64

static DRAM_ATTR std::atomic<uint16_t> bleChannelQueue[BLE_CHANNEL_QUEUE_SIZE];
static DRAM_ATTR uint32_t bleChannelQueueWriteIndex;
static DRAM_ATTR std::atomic<bool> bleChannelInfoTriggeredAtLeastOnce;
static DRAM_ATTR bool bleShouldSaveChannel;
static DRAM_ATTR std::atomic<unsigned long> bleDiscardedChannelInfo;
static uint32_t bleChannelQueueReadIndex;

extern "C" void IRAM_ATTR __wrap_r_ke_msg_send(void *msg) {
    bleShouldSaveChannel = true;
    __real_r_ke_msg_send(msg);
}

extern "C" void IRAM_ATTR __wrap_r_lld_scan_process_pkt_rx_adv_rep(int scan_index, unsigned long param_2,int rxdesc_index, void *param_4) {
    bleChannelInfoTriggeredAtLeastOnce = true;
    auto rxdesc = (const BleRxDescriptor*)r_emi_get_mem_addr_by_offset(BLE_RXDESC_OFFSET) + rxdesc_index;
    uint16_t channelAndRssi = rxdesc->channel << 8 | rxdesc->rssi;
    bleShouldSaveChannel = false;
    __real_r_lld_scan_process_pkt_rx_adv_rep(scan_index, param_2, rxdesc_index, param_4);
    if (!bleShouldSaveChannel) {
        bleDiscardedChannelInfo++;
        return;
    }
    bleChannelQueue[bleChannelQueueWriteIndex % BLE_CHANNEL_QUEUE_SIZE] = channelAndRssi;
    bleChannelQueueWriteIndex++;
}

namespace ChannelInfo {

uint8_t popChannelInfo(int8_t expectedRssi) {
    unsigned long discarded = bleDiscardedChannelInfo.exchange(0);
    if (discarded)
        Serial.printf("discarded %d rx pkts?\r\n", discarded);
    // Ignore the queue if it hasn't been touched by the hook, e.g. due to
    // unsupported hardware/firmware changes that would cause
    // r_lld_scan_process_pkt_rx_adv_rep not to get called at all.
    if (bleChannelInfoTriggeredAtLeastOnce) {
        //Serial.printf("wr: %lu, rd: %lu\r\n", (uint32_t)bleChannelQueueWriteIndex, (uint32_t)bleChannelQueueReadIndex);
        const auto initialReadIndex = bleChannelQueueReadIndex;
        do {
            uint16_t channelInfo = bleChannelQueue[bleChannelQueueReadIndex % BLE_CHANNEL_QUEUE_SIZE];
            bleChannelQueueReadIndex++;
            // The Bluetooth stack may have discarded some advertisements after
            // we've captured the channel value, skip ahead until we find a
            // matching RSSI value.
            if ((int8_t)(channelInfo & 0xff) == expectedRssi)
                return channelInfo >> 8;
        } while (bleChannelQueueReadIndex % BLE_CHANNEL_QUEUE_SIZE != initialReadIndex % BLE_CHANNEL_QUEUE_SIZE);
    }
    // Either the hook isn't working or we probably received more than
    // BLE_CHANNEL_QUEUE_SIZE packets since the last call to popChannelInfo(),
    // i.e. the channel info we were looking for is gone.
    return 37;
}

}
