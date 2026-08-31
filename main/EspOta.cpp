#include "EspOta.h"

#include <lwip/sockets.h>
#include <unistd.h>

#include <cstring>

#include "GUI.h"
#include "HttpWebServer.h"
#include "Logger.h"
#include "defaults.h"
#include "esp_ota_ops.h"
#include "mbedtls/md5.h"
#include "util.h"

namespace EspOta {
namespace {

// espota handshake: "<cmd> <host_port> <size> <md5>\n" over UDP; we answer "OK", then pull the
// image over TCP from the sender, acking each chunk with the byte count and "OK" at the end.
void runUpdate(const sockaddr_in& host, uint16_t port, size_t size, const char* md5hex) {
    const esp_partition_t* part = esp_ota_get_next_update_partition(nullptr);
    if (!part || size > part->size) {
        Log.printf("espota: no partition for %u bytes\r\n", (unsigned)size);
        return;
    }
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr = host;
    addr.sin_port = htons(port);
    struct timeval tv = {10, 0};
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
        Log.println("espota: connect to host failed");
        close(sock);
        return;
    }
    GUI::Update(UPDATE_STARTED);
    HttpWebServer::UpdateStart();

    esp_ota_handle_t ota = 0;
    esp_err_t err = esp_ota_begin(part, size, &ota);
    mbedtls_md5_context md5;
    mbedtls_md5_init(&md5);
    mbedtls_md5_starts(&md5);
    static uint8_t buf[1460];
    size_t written = 0;
    int lastPct = -1;
    while (err == ESP_OK && written < size) {
        int n = recv(sock, buf, sizeof(buf), 0);
        if (n <= 0) {
            err = ESP_ERR_TIMEOUT;
            break;
        }
        err = esp_ota_write(ota, buf, n);
        mbedtls_md5_update(&md5, buf, n);
        written += n;
        char ack[16];
        int len = snprintf(ack, sizeof(ack), "%u", (unsigned)written);
        send(sock, ack, len, 0);
        int pct = written * 100 / size;
        if (pct / 10 != lastPct / 10) {
            lastPct = pct;
            GUI::Update(pct);
        }
    }
    uint8_t digest[16];
    mbedtls_md5_finish(&md5, digest);
    mbedtls_md5_free(&md5);
    char got[33];
    for (int i = 0; i < 16; i++) snprintf(got + i * 2, 3, "%02x", digest[i]);

    if (err == ESP_OK && strcasecmp(got, md5hex) != 0) {
        Log.printf("espota: md5 mismatch %s != %s\r\n", got, md5hex);
        err = ESP_ERR_INVALID_CRC;
    }
    if (err == ESP_OK) err = esp_ota_end(ota);
    else esp_ota_abort(ota);
    if (err == ESP_OK) err = esp_ota_set_boot_partition(part);

    GUI::Update(UPDATE_COMPLETE);
    HttpWebServer::UpdateEnd();
    if (err == ESP_OK) {
        send(sock, "OK", 2, 0);
        Log.println("espota: update OK, rebooting");
        delay(200);
        close(sock);
        esp_restart();
    }
    char msg[64];
    int len = snprintf(msg, sizeof(msg), "ERR: %s", esp_err_to_name(err));
    send(sock, msg, len, 0);
    Log.printf("espota: %s\r\n", msg);
    close(sock);
}

void task(void* arg) {
    uint16_t port = (uint16_t)(uintptr_t)arg;
    int udp = socket(AF_INET, SOCK_DGRAM, 0);
    sockaddr_in bindAddr = {};
    bindAddr.sin_family = AF_INET;
    bindAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bindAddr.sin_port = htons(port);
    if (bind(udp, (sockaddr*)&bindAddr, sizeof(bindAddr)) < 0) {
        Log.println("espota: bind failed");
        close(udp);
        vTaskDelete(nullptr);
        return;
    }
    char pkt[128];
    while (true) {
        sockaddr_in from;
        socklen_t fromLen = sizeof(from);
        int n = recvfrom(udp, pkt, sizeof(pkt) - 1, 0, (sockaddr*)&from, &fromLen);
        if (n <= 0) continue;
        pkt[n] = 0;
        unsigned cmd, hostPort, size;
        char md5hex[40];
        if (sscanf(pkt, "%u %u %u %39s", &cmd, &hostPort, &size, md5hex) != 4 || strlen(md5hex) != 32) continue;
        if (cmd != 0) {  // 0 = flash; 100 = filesystem, which we do not carry
            sendto(udp, "ERR: only flash", 15, 0, (sockaddr*)&from, fromLen);
            continue;
        }
        // ponytail: no OTA password, same as the Arduino build (ArduinoOTA.setPassword was never called).
        sendto(udp, "OK", 2, 0, (sockaddr*)&from, fromLen);
        Log.printf("espota: update from %s (%u bytes)\r\n", inet_ntoa(from.sin_addr), size);
        runUpdate(from, hostPort, size, md5hex);
    }
}
}  // namespace

void Start(uint16_t port) {
    static bool started = false;
    if (started) return;
    started = true;
    xTaskCreate(task, "espota", 4096, (void*)(uintptr_t)port, 1, nullptr);
}
}  // namespace EspOta
