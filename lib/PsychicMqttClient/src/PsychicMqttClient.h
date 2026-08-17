#pragma once
/**
 *   PsychicMqttClient
 *
 *   Fully featured async MQTT 3.1.1 client for ESP32 with support for SSL/TLS
 *   and MQTT over WS. Uses the ESP-IDF MQTT client library under the hood and
 *   adds a powerful but easy to use API on top of it. Supports MQTT over TCP,
 *   SSL with mbedtls, MQTT over Websocket and MQTT over Websocket Secure.
 *   https://github.com/theelims/PsychicMqttClient
 *
 *   MIT License
 *
 *   Copyright (c) 2024 elims
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in all
 *   copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *   SOFTWARE.
 */

#include <functional>
#include <vector>

#include "Arduino.h"
#include "mqtt_client.h"
#include "esp_crt_bundle.h"

#define PSYCHIC_MQTT_CLIENT_VERSION_STR "0.2.1"
#define PSYCHIC_MQTT_CLIENT_VERSION_MAJOR 0
#define PSYCHIC_MQTT_CLIENT_VERSION_MINOR 2
#define PSYCHIC_MQTT_CLIENT_VERSION_PATCH 1

#ifndef ARDUINO_ARCH_ESP32
#error "This library only supports boards with an ESP32 processor."
#endif

// user callbacks
typedef std::function<void(bool sessionPresent)> OnConnectUserCallback;
typedef std::function<void(bool sessionPresent)> OnDisconnectUserCallback;
typedef std::function<void(int msgId)> OnSubscribeUserCallback;
typedef std::function<void(int msgId)> OnUnsubscribeUserCallback;
typedef std::function<void(char *topic, char *payload, int retain, int qos, bool dup)> OnMessageUserCallback;
typedef std::function<void(int msgId)> OnPublishUserCallback;
typedef std::function<void(esp_mqtt_error_codes_t error)> OnErrorUserCallback;

typedef struct
{
    char *topic;
    int qos;
    OnMessageUserCallback callback;
} OnMessageUserCallback_t;

/**
 * @class PsychicMqttClient
 * @brief A class that wraps the ESP-IDF MQTT client and provides a more user friendly interface.
 * The API is very similar to AsyncMqttClient for the ESP32 by Marvin Roger, so that this library can
 * be used as a almost drop-in replacement.
 */
class PsychicMqttClient
{
public:
    /**
     * @brief Constructs a new instance of the PsychicMqttClient class.
     */
    PsychicMqttClient();

    /**
     * @brief Destroys the instance of the PsychicMqttClient class.
     */
    ~PsychicMqttClient();

    /**
     * @brief Sets the keep alive interval in seconds for the MQTT connection.
     *
     * @param keepAlive The keep alive interval in seconds. Defaults to 120.
     * @return A reference to the PsychicMqttClient instance.
     */
    PsychicMqttClient &setKeepAlive(int keepAlive = 120);

    /**
     * @brief Sets the auto reconnect flag for the MQTT connection.
     *
     * @param reconnect The auto reconnect flag. Defaults to true.
     * @return A reference to the PsychicMqttClient instance.
     */
    PsychicMqttClient &setAutoReconnect(bool reconnect = true);

    /**
     * @brief Sets the client ID for the MQTT connection.
     *
     * @param clientId The client ID. Defaults to ESP32_%CHIPID% where
     * %CHIPID% are the last 3 bytes of MAC address in hex format.
     * @return A reference to the PsychicMqttClient instance.
     */
    PsychicMqttClient &setClientId(const char *clientId);

    /**
     * @brief Sets the clean session flag for the MQTT connection.
     *
     * @param cleanSession The clean session flag. Defaults to true.
     * @return A reference to the PsychicMqttClient instance.
     */
    PsychicMqttClient &setCleanSession(bool cleanSession = true);

    /**
     * @brief Sets the size for the MQTT send/receive buffer. If messages exceed
     * the buffer size, the message will be split into multiple chunks. Received m
     * essages will be assembled into the original message.
     *
     * @param bufferSize The buffer size in bytes. Defaults to 1024.
     * @return A reference to the PsychicMqttClient instance.
     */
    PsychicMqttClient &setBufferSize(int bufferSize = 1024);

    /**
     * @brief Sets the task stack size and priority for the MQTT client task.
     *
     * @param stackSize The task stack size in bytes. Defaults to 6144.
     * @param prio The task priority. Defaults to 5.
     * @return A reference to the PsychicMqttClient instance.
     */
    PsychicMqttClient &setTaskStackAndPriority(int stackSize, int prio);

    /**
     * @brief Sets the CA root certificate for the MQTT server.
     *
     * @param cert The certificate in PEM or DER format.
     * @param certLen optional length of the certificate shouldn't cert be null-terminated.
     * @return A reference to the PsychicMqttClient instance.
     */
    PsychicMqttClient &setCACert(const char *rootCA, size_t rootCALen = 0);

    /**
     * @brief Sets a CA root certificate bundle for the MQTT server. Use this
     * method if you have multiple CA root certificates and this is the only place
     * using SSL/TLS. Otherwise use attachArduinoCACertBundle() to attach an existing
     * certificate bundle.
     *
     * @param bundle The certificate bundle in PEM or DER format.
     * @return A reference to the PsychicMqttClient instance.
     */
    PsychicMqttClient &setCACertBundle(const uint8_t *bundle, size_t bundleLen = 0);

    /**
     * @brief Attaches an existing CA root certificate bundle for the MQTT server. Like if you
     * already use WiFiClientSecure and want to use the same CA root certificate bundle for MQTT.
     *
     * @param attach Whether to attach or detach the CA root certificate bundle. Defaults to true.
     * @return A reference to the PsychicMqttClient instance.
     */
    PsychicMqttClient &attachArduinoCACertBundle(bool attach = true);

    /**
     * @brief Sets the credentials for the MQTT connection.
     *
     * @param username The username for the MQTT connection. Defaults to nullptr.
     * @param password The password for the MQTT connection. Defaults to nullptr.
     * @return A reference to the PsychicMqttClient instance.
     */
    PsychicMqttClient &setCredentials(const char *username, const char *password = nullptr);

    /**
     * @brief Authenticate via mutual X.509 certificate.
     *
     * @param clientCert The X.509 certificate issued from the same CA as the server cert.
     * @param clientKey The private key for the X.509 certificate.
     * @param clientCertLen Optional length of the client certificate. If the certificate is not null-terminated.
     * @param clientKeyLen Optional length of the client key. If the key is not null-terminated.
     * @return A reference to the PsychicMqttClient instance.
     */
    PsychicMqttClient &setClientCertificate(const char *clientCert, const char *clientKey, size_t clientCertLen = 0,
        size_t clientKeyLen = 0);

    /**
     * @brief Sets the last will and testament for the MQTT connection.
     *
     * @param topic The topic for the last will and testament.
     * @param qos The QoS level for the last will and testament.
     * @param retain The retain flag for the last will and testament.
     * @param payload The payload for the last will and testament. Defaults to nullptr.
     * @param length The length of the payload for the last will and testament. Defaults to 0.
     * @return A reference to the PsychicMqttClient instance.
     */
    PsychicMqttClient &setWill(const char *topic, uint8_t qos, bool retain, const char *payload = nullptr, int length = 0);

    /**
     * @brief Sets the MQTT server URI. Supports mqtt://, mqtts:// and ws://, wss:// as
     * transport protocols. Fully supports SSL/TLS.
     *
     * Example: mqtt://162.168.10.1
     *          mqtt://mqtt.eclipseprojects.io
     *          mqtt://mqtt.eclipseprojects.io:1884
     *          mqtt://username:password@mqtt.eclipseprojects.io:1884
     *          mqtts://mqtt.eclipseprojects.io
     *          mqtts://mqtt.eclipseprojects.io:8884
     *          ws://mqtt.eclipseprojects.io:80/mqtt
     *          wss://mqtt.eclipseprojects.io:443/mqtt
     *
     * @param uri The MQTT server URI.
     * @return A reference to the PsychicMqttClient instance.
     */
    PsychicMqttClient &setServer(const char *uri);

    /**
     * @brief Registers a callback function to be called when the MQTT client is connected.
     *
     * @param callback The callback function with the signature void(bool sessionPresent) to be registered.
     * @return A reference to the PsychicMqttClient instance.
     */
    PsychicMqttClient &onConnect(OnConnectUserCallback callback);

    /**
     * @brief Registers a callback function to be called when the MQTT client is disconnected.
     *
     * @param callback The callback function with the signature void(bool sessionPresent) to
     * be registered.
     * @return A reference to the PsychicMqttClient instance.
     */
    PsychicMqttClient &onDisconnect(OnDisconnectUserCallback callback);

    /**
     * @brief Registers a callback function to be called when a topic is subscribed.
     *
     * @param callback The callback function with the signature void(int msgId) to be registered.
     * @return A reference to the PsychicMqttClient instance.
     */
    PsychicMqttClient &onSubscribe(OnSubscribeUserCallback callback);

    /**
     * @brief Registers a callback function to be called when a topic is unsubscribed.
     *
     * @param callback The callback function with the signature void(int msgId) to be registered.
     * @return A reference to the PsychicMqttClient instance.
     */
    PsychicMqttClient &onUnsubscribe(OnUnsubscribeUserCallback callback);

    /**
     * @brief Registers a callback function to be called when a message is received.
     * Multipart messages will be reassembled into the original message.
     *
     * @param callback The callback function with the signature void(char *topic,
     * char *payload, int msgId, int retain, int qos, bool dup) to be registered.
     * @return A reference to the PsychicMqttClient instance.
     */
    PsychicMqttClient &onMessage(OnMessageUserCallback callback);

    /**
     * @brief Registers a callback function to be called when a message is
     * received on a specific topic. Multipart messages will be
     * reassembled into the original message. Fully supports MQTT Wildcards.
     * Will automatically subscribe to all topics once the client is connected.
     *
     * @param topic The topic to listen for. MQTT Wildcards are fully supported.
     * @param qos The QoS level to listen for.
     * @param callback The callback function with the signature void(char *topic,
     * char *payload, int msgId, int retain, int qos, bool dup) to be registered.
     * @return A reference to the PsychicMqttClient instance.
     */
    PsychicMqttClient &onTopic(const char *topic, int qos, OnMessageUserCallback callback);

    /**
     * @brief Registers a callback function to be called when a message is published.
     *
     * @param callback The callback function with the signature void(int msgId) to be registered.
     * @return A reference to the PsychicMqttClient instance.
     */
    PsychicMqttClient &onPublish(OnPublishUserCallback callback);

    /**
     * @brief Registers a callback function to be called when an error occurs.
     *
     * @param callback The callback function with the signature void(esp_mqtt_error_codes_t error) to be registered.
     * @return A reference to the PsychicMqttClient instance.
     */
    PsychicMqttClient &onError(OnErrorUserCallback callback);

    /**
     * @brief Checks if the MQTT client is connected.
     *
     * @return True if the client is connected, false otherwise.
     */
    bool connected();

    /**
     * @brief Connects the MQTT client to the server.
     *
     * @note All parameters must be set before calling this method.
     */
    void connect();

    /**
     * @brief Disconnects the MQTT client from the server.
     * This call might be blocking until the client is stopped cleanly
     */
    void disconnect();

    /**
     * @brief Forcefully stops the MQTT client and disconnects from the server.
     * This does not trigger the onDisconnect callbacks.
     */
    void forceStop();

    /**
     * @brief Subscribes to a topic. Server must be connected
     * for a subscription to succeed.
     *
     * @param topic The topic to subscribe to.
     * @param qos The QoS level for the subscription.
     * @return Message ID on success, -1 on failure.
     */
    int subscribe(const char *topic, int qos);

    /**
     * @brief Unsubscribes from a topic. Server must be connected
     * for an unsubscription to succeed.
     *
     * @param topic The topic to unsubscribe from.
     * @return Message ID on success, -1 on failure.
     */
    int unsubscribe(const char *topic);

    /**
     * @brief Publishes a message to a topic.
     *
     * @param topic The topic to publish to.
     * @param qos The QoS level (0-2) for the message.
     * @param retain The retain flag for the message.
     * @param payload The payload for the message. Defaults to nullptr.
     * @param length The length of the payload. Defaults to 0.
     * @param async Whether to enqueue the message for asynchronous publishing.
     * Defaults to true. False means blocking until the message is published.
     * @return Message ID on success, -1 on failure.
     */
    int publish(const char *topic, int qos, bool retain, const char *payload = nullptr, int length = 0, bool async = true);

    /**
     * @brief Gets the client ID of the MQTT client.
     *
     * @return The client ID.
     */
    const char *getClientId();

    /**
     * @brief Returns the ESP-IDF MQTT client config object of the MQTT client in case
     * lower level access is needed.
     *
     * @return The config object.
     */
    esp_mqtt_client_config_t *getMqttConfig();

private:
    esp_mqtt_client_handle_t _client = nullptr;
    esp_mqtt_client_config_t _mqtt_cfg;
    esp_mqtt_error_codes_t _lastError;
    bool _connected = false;
    bool _stopMqttClient = false;

    char *_buffer = nullptr;
    char *_topic = nullptr;

    static void _onMqttEventStatic(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
    void _onMqttEvent(esp_event_base_t base, int32_t event_id, void *event_data);
    bool _isTopicMatch(const char *topic, const char *subscription);

    std::vector<OnConnectUserCallback> _onConnectUserCallbacks;
    std::vector<OnDisconnectUserCallback> _onDisconnectUserCallbacks;
    std::vector<OnSubscribeUserCallback> _onSubscribeUserCallbacks;
    std::vector<OnUnsubscribeUserCallback> _onUnsubscribeUserCallbacks;
    std::vector<OnMessageUserCallback_t> _onMessageUserCallbacks;
    std::vector<OnPublishUserCallback> _onPublishUserCallbacks;
    std::vector<OnErrorUserCallback> _onErrorUserCallbacks;

    void _onBeforeConnect(esp_mqtt_event_handle_t &event_data, esp_mqtt_client_handle_t &client);
    void _onConnect(esp_mqtt_event_handle_t &event_data);
    void _onDisconnect(esp_mqtt_event_handle_t &event_data);
    void _onSubscribe(esp_mqtt_event_handle_t &event_data);
    void _onUnsubscribe(esp_mqtt_event_handle_t &event_data);
    void _onMessage(esp_mqtt_event_handle_t &event_data);
    void _onPublish(esp_mqtt_event_handle_t &event_data);
    void _onError(esp_mqtt_event_handle_t &event_data);
};
