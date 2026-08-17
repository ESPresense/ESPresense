#include "PsychicMqttClient.h"

static const char *TAG = "🐙";

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0)
    {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

PsychicMqttClient::PsychicMqttClient() : _mqtt_cfg()
{
    memset(&_mqtt_cfg, 0, sizeof(_mqtt_cfg));
}

PsychicMqttClient::~PsychicMqttClient()
{
    disconnect();
    esp_mqtt_client_destroy(_client);

    // Free memory in _buffer and _topic
    if (_buffer != nullptr)
    {
        free(_buffer);
        _buffer = nullptr; // Set to nullptr to avoid dangling pointers
    }

    if (_topic != nullptr)
    {
        free(_topic);
        _topic = nullptr; // Set to nullptr to avoid dangling pointers
    }

    // Free memory in _onMessageUserCallbacks
    for (auto &callback : _onMessageUserCallbacks)
    {
        if (callback.topic != nullptr)
        {
            free(callback.topic); // Free the dynamically allocated topic
            callback.topic = nullptr;
        }
    }
    _onMessageUserCallbacks.clear(); // Clear the vector
}

PsychicMqttClient &PsychicMqttClient::setKeepAlive(int keepAlive)
{
#if ESP_IDF_VERSION_MAJOR == 5
    _mqtt_cfg.session.keepalive = keepAlive;
#else
    _mqtt_cfg.keepalive = keepAlive;
#endif
    return *this;
}

PsychicMqttClient &PsychicMqttClient::setAutoReconnect(bool reconnect)
{
#if ESP_IDF_VERSION_MAJOR == 5
    _mqtt_cfg.network.disable_auto_reconnect = !reconnect;
#else
    _mqtt_cfg.disable_auto_reconnect = !reconnect;
#endif
    return *this;
}

PsychicMqttClient &PsychicMqttClient::setClientId(const char *clientId)
{
#if ESP_IDF_VERSION_MAJOR == 5
    _mqtt_cfg.credentials.client_id = clientId;
#else
    _mqtt_cfg.client_id = clientId;
#endif
    return *this;
}

PsychicMqttClient &PsychicMqttClient::setCleanSession(bool cleanSession)
{
#if ESP_IDF_VERSION_MAJOR == 5
    _mqtt_cfg.session.disable_clean_session = !cleanSession;
#else
    _mqtt_cfg.disable_clean_session = !cleanSession;
#endif
    return *this;
}

PsychicMqttClient &PsychicMqttClient::setBufferSize(int bufferSize)
{
#if ESP_IDF_VERSION_MAJOR == 5
    _mqtt_cfg.buffer.size = bufferSize;
#else
    _mqtt_cfg.buffer_size = bufferSize;
#endif
    return *this;
}

PsychicMqttClient &PsychicMqttClient::setTaskStackAndPriority(int stackSize, int priority)
{
#if ESP_IDF_VERSION_MAJOR == 5
    _mqtt_cfg.task.stack_size = stackSize;
    _mqtt_cfg.task.priority = priority;
#else
    _mqtt_cfg.task_stack = stackSize;
    _mqtt_cfg.task_prio = priority;
#endif
    return *this;
}

PsychicMqttClient &PsychicMqttClient::setCACert(const char *rootCA, size_t rootCALen)
{
#if ESP_IDF_VERSION_MAJOR == 5
    _mqtt_cfg.broker.verification.certificate = rootCA;
    _mqtt_cfg.broker.verification.certificate_len = rootCALen;
#else
    _mqtt_cfg.cert_pem = rootCA;
    _mqtt_cfg.cert_len = rootCALen;
#endif
    return *this;
}

PsychicMqttClient &PsychicMqttClient::setCACertBundle(const uint8_t *bundle, size_t bundleLen)
{
#if ESP_IDF_VERSION_MAJOR == 5
    if (bundle != nullptr)
    {
        esp_crt_bundle_set(bundle, bundleLen);
        _mqtt_cfg.broker.verification.crt_bundle_attach = esp_crt_bundle_attach;
    }
    else
    {
        esp_crt_bundle_detach(NULL);
        _mqtt_cfg.broker.verification.crt_bundle_attach = NULL;
    }
#else
    if (bundle != nullptr)
    {
        arduino_esp_crt_bundle_set(bundle);
        _mqtt_cfg.crt_bundle_attach = arduino_esp_crt_bundle_attach;
    }
    else
    {
        arduino_esp_crt_bundle_detach(NULL);
        _mqtt_cfg.crt_bundle_attach = NULL;
    }
#endif
    return *this;
}

PsychicMqttClient &PsychicMqttClient::attachArduinoCACertBundle(bool attach)
{
#if ESP_IDF_VERSION_MAJOR == 5
    if (attach)
        _mqtt_cfg.broker.verification.crt_bundle_attach = esp_crt_bundle_attach;
    else
        _mqtt_cfg.broker.verification.crt_bundle_attach = NULL;
#else
    if (attach)
        _mqtt_cfg.crt_bundle_attach = arduino_esp_crt_bundle_attach;
    else
        _mqtt_cfg.crt_bundle_attach = NULL;
#endif
    return *this;
}

PsychicMqttClient &PsychicMqttClient::setCredentials(const char *username, const char *password)
{
#if ESP_IDF_VERSION_MAJOR == 5
    _mqtt_cfg.credentials.username = username;
    if (password != nullptr)
        _mqtt_cfg.credentials.authentication.password = password;
#else
    _mqtt_cfg.username = username;
    if (password != nullptr)
        _mqtt_cfg.password = password;
#endif
    return *this;
}

PsychicMqttClient &PsychicMqttClient::setClientCertificate(const char *clientCert, const char *clientKey,
                                                           size_t clientCertLen, size_t clientKeyLen)
{
#if ESP_IDF_VERSION_MAJOR == 5
    _mqtt_cfg.credentials.authentication.certificate = clientCert;
    _mqtt_cfg.credentials.authentication.key = clientKey;
    _mqtt_cfg.credentials.authentication.certificate_len = clientCertLen;
    _mqtt_cfg.credentials.authentication.key_len = clientKeyLen;
#else
    _mqtt_cfg.client_cert_pem = clientCert;
    _mqtt_cfg.client_key_pem = clientKey;
    _mqtt_cfg.client_cert_len = clientCertLen;
    _mqtt_cfg.client_key_len = clientKeyLen;
#endif
    return *this;
}

PsychicMqttClient &PsychicMqttClient::setWill(const char *topic, uint8_t qos, bool retain, const char *payload, int length)
{
#if ESP_IDF_VERSION_MAJOR == 5
    _mqtt_cfg.session.last_will.topic = topic;
    _mqtt_cfg.session.last_will.qos = qos;
    _mqtt_cfg.session.last_will.retain = retain;
    _mqtt_cfg.session.last_will.msg_len = length;
    _mqtt_cfg.session.last_will.msg = payload;
#else
    _mqtt_cfg.lwt_topic = topic;
    _mqtt_cfg.lwt_qos = qos;
    _mqtt_cfg.lwt_retain = retain;
    _mqtt_cfg.lwt_msg_len = length;
    _mqtt_cfg.lwt_msg = payload;
#endif
    return *this;
}

PsychicMqttClient &PsychicMqttClient::setServer(const char *uri)
{
#if ESP_IDF_VERSION_MAJOR == 5
    _mqtt_cfg.broker.address.uri = uri;
#else
    _mqtt_cfg.uri = uri;
#endif
    return *this;
}

PsychicMqttClient &PsychicMqttClient::onConnect(OnConnectUserCallback callback)
{
    _onConnectUserCallbacks.push_back(callback);
    return *this;
}

PsychicMqttClient &PsychicMqttClient::onDisconnect(OnDisconnectUserCallback callback)
{
    _onDisconnectUserCallbacks.push_back(callback);
    return *this;
}

PsychicMqttClient &PsychicMqttClient::onSubscribe(OnSubscribeUserCallback callback)
{
    _onSubscribeUserCallbacks.push_back(callback);
    return *this;
}

PsychicMqttClient &PsychicMqttClient::onUnsubscribe(OnUnsubscribeUserCallback callback)
{
    _onUnsubscribeUserCallbacks.push_back(callback);
    return *this;
}

PsychicMqttClient &PsychicMqttClient::onMessage(OnMessageUserCallback callback)
{
    OnMessageUserCallback_t subscription = {nullptr, 0, callback};
    _onMessageUserCallbacks.push_back(subscription);
    return *this;
}

PsychicMqttClient &PsychicMqttClient::onTopic(const char *topic, int qos, OnMessageUserCallback callback)
{
    OnMessageUserCallback_t subscription = {strcpy((char *)malloc(strlen(topic) + 1), topic), qos, callback};
    _onMessageUserCallbacks.push_back(subscription);
    if (_connected)
        subscribe(topic, qos);
    return *this;
}

PsychicMqttClient &PsychicMqttClient::onPublish(OnPublishUserCallback callback)
{
    _onPublishUserCallbacks.push_back(callback);
    return *this;
}

PsychicMqttClient &PsychicMqttClient::onError(OnErrorUserCallback callback)
{
    _onErrorUserCallbacks.push_back(callback);
    return *this;
}

bool PsychicMqttClient::connected()
{
    return _connected;
}

void PsychicMqttClient::connect()
{
#if ESP_IDF_VERSION_MAJOR == 5
    if (_mqtt_cfg.broker.address.uri == nullptr)
    {
        ESP_LOGE(TAG, "MQTT URI not set.");
        return;
    }
#else
    if (_mqtt_cfg.uri == nullptr)
    {
        ESP_LOGE(TAG, "MQTT URI not set.");
        return;
    }
#endif

    if (_client == nullptr)
        _client = esp_mqtt_client_init(&_mqtt_cfg);
    else
        ESP_ERROR_CHECK_WITHOUT_ABORT(esp_mqtt_set_config(_client, &_mqtt_cfg));

    esp_mqtt_client_register_event(_client, MQTT_EVENT_ANY, _onMqttEventStatic, this);
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_mqtt_client_start(_client));
    ESP_LOGI(TAG, "MQTT client started.");
}

void PsychicMqttClient::disconnect()
{
    if (_client == nullptr)
    {
        ESP_LOGW(TAG, "MQTT client not started.");
        return;
    }

    if (_connected)
    {
        ESP_LOGI(TAG, "Disconnecting MQTT client.");
        _stopMqttClient = false;
        esp_mqtt_client_disconnect(_client);

        // Wait for all disconnect events to be processed
        while (!_stopMqttClient)
        {
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
    }

    esp_mqtt_client_stop(_client);
    ESP_LOGI(TAG, "MQTT client stopped.");
}

void PsychicMqttClient::forceStop()
{
    if (_client == nullptr)
    {
        ESP_LOGW(TAG, "MQTT client not started.");
        return;
    }

    if (_connected)
    {
        ESP_LOGI(TAG, "Forced stop MQTT client.");
    }
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_mqtt_client_stop(_client));
    _connected = false;
    ESP_LOGI(TAG, "MQTT client forcefully stopped.");
}

int PsychicMqttClient::subscribe(const char *topic, int qos)
{
    if (_connected)
    {
        ESP_LOGI(TAG, "Subscribing to topic %s with QoS %d", topic, qos);
        return esp_mqtt_client_subscribe(_client, topic, qos);
    }
    else
    {
        ESP_LOGW(TAG, "MQTT client not connected. Dropping subscription to topic %s with QoS %d.", topic, qos);
        return -1;
    }
}

int PsychicMqttClient::unsubscribe(const char *topic)
{
    ESP_LOGI(TAG, "Unsubscribing from topic %s", topic);
    return esp_mqtt_client_unsubscribe(_client, topic);
}

int PsychicMqttClient::publish(const char *topic, int qos, bool retain, const char *payload, int length, bool async)
{
    // drop message if not connected and QoS is 0
    if (!connected() && qos == 0)
    {
        ESP_LOGW(TAG, "MQTT client not connected. Dropping message with QoS = 0.");
        return -1;
    }

    if (async)
    {
        ESP_LOGV(TAG, "Enqueuing message to topic %s with QoS %d", topic, qos);
        return esp_mqtt_client_enqueue(_client, topic, payload, length, qos, retain, true);
    }
    else
    {
        ESP_LOGV(TAG, "Publishing message to topic %s with QoS %d", topic, qos);
        return esp_mqtt_client_publish(_client, topic, payload, length, qos, retain);
    }
}

const char *PsychicMqttClient::getClientId()
{
#if ESP_IDF_VERSION_MAJOR == 5
    return _mqtt_cfg.credentials.client_id;
#else
    return _mqtt_cfg.client_id;
#endif
}

esp_mqtt_client_config_t *PsychicMqttClient::getMqttConfig()
{
    return &_mqtt_cfg;
}

void PsychicMqttClient::_onMqttEventStatic(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    // Since this is a static function, we need to cast the first argument (void*) back to the class instance type
    PsychicMqttClient *instance = (PsychicMqttClient *)handler_args;
    instance->_onMqttEvent(base, event_id, event_data);
}

void PsychicMqttClient::_onMqttEvent(esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGV(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
    switch (event_id)
    {
    case MQTT_EVENT_CONNECTED:
        _connected = true;
        _onConnect(event);
        break;
    case MQTT_EVENT_DISCONNECTED:
        _connected = false;
        _onDisconnect(event);
        break;
    case MQTT_EVENT_SUBSCRIBED:
        _onSubscribe(event);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        _onUnsubscribe(event);
        break;
    case MQTT_EVENT_PUBLISHED:
        _onPublish(event);
        break;
    case MQTT_EVENT_DATA:
        _onMessage(event);
        break;
    case MQTT_EVENT_ERROR:
        _connected = false;
        _onError(event);
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

void PsychicMqttClient::_onConnect(esp_mqtt_event_handle_t &event)
{
    ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");

    // Resubscribe to all topics
    for (auto topic : _onMessageUserCallbacks)
    {
        if (topic.topic != nullptr)
            subscribe(topic.topic, topic.qos);
    }

    for (auto callback : _onConnectUserCallbacks)
    {
        callback(event->session_present);
    }
}

void PsychicMqttClient::_onDisconnect(esp_mqtt_event_handle_t &event)
{
    ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
    for (auto callback : _onDisconnectUserCallbacks)
    {
        callback(event->session_present);
    }
    _stopMqttClient = true;
}

void PsychicMqttClient::_onSubscribe(esp_mqtt_event_handle_t &event)
{
    ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
    for (auto callback : _onSubscribeUserCallbacks)
    {
        callback(event->msg_id);
    }
}

void PsychicMqttClient::_onUnsubscribe(esp_mqtt_event_handle_t &event)
{
    ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
    for (auto callback : _onUnsubscribeUserCallbacks)
    {
        callback(event->msg_id);
    }
}

void PsychicMqttClient::_onMessage(esp_mqtt_event_handle_t &event)
{
    // ESP_LOGI(TAG, "MQTT_EVENT_DATA");
    // printf("MSG_ID=%d\r\n", event->msg_id);
    // printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
    // printf("DATA=%.*s\r\n", event->data_len, event->data);
    // printf("DATA_LEN=%d\r\n", event->data_len);
    // printf("TOTAL_DATA_LEN=%d\r\n", event->total_data_len);
    // printf("CURRENT_DATA_OFFSET=%d\r\n", event->current_data_offset);

    // Check if we are dealing with a simple message
    if (event->total_data_len == event->data_len)
    {
        ESP_LOGV(TAG, "MQTT_EVENT_DATA_SINGLE");
        // Copy the characters from data->data_ptr to c-string
        char payload[event->data_len + 1];
        memcpy(payload, (char *)event->data, event->data_len);
        payload[event->data_len] = '\0';
        ESP_LOGV(TAG, "Payload=%s", payload);

        char topic[event->topic_len + 1];
        memcpy(topic, (char *)event->topic, event->topic_len);
        topic[event->topic_len] = '\0';
        ESP_LOGV(TAG, "Topic=%s", topic);

        for (auto callback : _onMessageUserCallbacks)
        {
            if (callback.topic == nullptr || _isTopicMatch(topic, callback.topic))
            {
                callback.callback(topic, payload, event->retain, event->qos, event->dup);
            }
        }
    }

    // Check if we are dealing with a first multipart message
    else if (event->current_data_offset == 0)
    {
        ESP_LOGV(TAG, "MQTT_EVENT_DATA_MULTIPART_FIRST");
        // Free any remnant of an unterminated multipart message
        free(_buffer);
        free(_topic);
        // Allocate memory for the buffer; on failure drop the whole message
        // instead of memcpy()ing into NULL (StoreProhibited panic)
        _buffer = (char *)malloc(event->total_data_len + 1);
        if (_buffer == nullptr)
        {
            ESP_LOGE(TAG, "Multipart payload malloc(%d) failed - dropping message on %.*s", event->total_data_len + 1, event->topic_len, (char *)event->topic);
            _topic = nullptr;
            return;
        }
        // Copy the characters from even->data to _buffer
        memcpy(_buffer, (char *)event->data, event->data_len);

        // Store the topic for later use, as it is only sent with the first message
        _topic = (char *)malloc(event->topic_len + 1);
        if (_topic == nullptr)
        {
            ESP_LOGE(TAG, "Multipart topic malloc(%d) failed - dropping message", event->topic_len + 1);
            free(_buffer);
            _buffer = nullptr;
            return;
        }
        memcpy(_topic, (char *)event->topic, event->topic_len);
        _topic[event->topic_len] = '\0';
    }

    // Check if we are on the last message
    else if (event->current_data_offset + event->data_len == event->total_data_len)
    {
        ESP_LOGV(TAG, "MQTT_EVENT_DATA_MULTIPART_LAST");
        // Reassembly of this message was aborted (allocation failure) or never started
        if (_buffer == nullptr || _topic == nullptr)
        {
            ESP_LOGW(TAG, "Dropping multipart message - reassembly buffer unavailable");
            return;
        }
        // Copy the characters from even->data to _buffer
        memcpy(_buffer + event->current_data_offset, (char *)event->data, event->data_len);
        _buffer[event->total_data_len] = '\0';
        ESP_LOGV(TAG, "Topic=%s", _topic);
        ESP_LOGV(TAG, "Payload=%s", _buffer);

        for (auto callback : _onMessageUserCallbacks)
        {
            if (callback.topic == nullptr || _isTopicMatch(_topic, callback.topic))
            {
                callback.callback(_topic, _buffer, event->retain, event->qos, event->dup);
            }
        }

        // Free the memory
        free(_buffer);
        _buffer = nullptr;
        free(_topic);
        _topic = nullptr;
    }

    // Otherwise, we are in the middle of the message
    else
    {
        // Skip fragments of a message whose reassembly was aborted
        if (_buffer == nullptr) return;
        // copy the characters from even->data to _buffer
        memcpy(_buffer + event->current_data_offset, (char *)event->data, event->data_len);
        ESP_LOGV(TAG, "MQTT_EVENT_DATA_MULTIPART");
    }
}

void PsychicMqttClient::_onPublish(esp_mqtt_event_handle_t &event)
{
    ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
    for (auto callback : _onPublishUserCallbacks)
    {
        callback(event->msg_id);
    }
}

void PsychicMqttClient::_onError(esp_mqtt_event_handle_t &event)
{
    ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
    if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
    {
        log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
        log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
        log_error_if_nonzero("captured as transport's socket errno", event->error_handle->esp_transport_sock_errno);
        ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));

        for (auto callback : _onErrorUserCallbacks)
        {
            callback(*event->error_handle);
        }
    }
}

bool PsychicMqttClient::_isTopicMatch(const char *topic, const char *subscription)
{
    ESP_LOGD(TAG, "Match topic: %s with subscription: %s", topic, subscription);

    String topicStr(topic);
    String subscriptionStr(subscription);

    // Check if the subscription is a pure wildcard
    if (subscriptionStr == "#" || subscriptionStr == "+")
    {
        ESP_LOGV(TAG, "Subscription is a pure wildcard --> MATCH");
        return true;
    }

    // Check if the topic is a simple match
    if (topicStr == subscriptionStr)
    {
        ESP_LOGV(TAG, "Topic is a direct match --> MATCH");
        return true;
    }

    // Split the topic and subscription into tokens
    int topicIndex = 0;
    int subscriptionIndex = 0;
    int lastTopicIndex = topicStr.lastIndexOf('/');
    int lastSubscriptionIndex = subscriptionStr.lastIndexOf('/');
    String topicToken = topicStr.substring(topicIndex, topicStr.indexOf('/', topicIndex));
    String subscriptionToken = subscriptionStr.substring(subscriptionIndex, subscriptionStr.indexOf('/', subscriptionIndex));

    ESP_LOGV(TAG, "Initial topic token: %s, subscription token: %s", topicToken.c_str(), subscriptionToken.c_str());
    ESP_LOGV(TAG, "Last topic index: %d, last subscription index: %d", lastTopicIndex, lastSubscriptionIndex);

    while (topicToken.length() > 0 && subscriptionToken.length() > 0)
    {
        ESP_LOGV(TAG, "Comparing topic token: %s with subscription token: %s", topicToken.c_str(), subscriptionToken.c_str());

        if (subscriptionToken == "#")
        {
            ESP_LOGV(TAG, "Subscription token is # --> MATCH");
            return true;
        }
        if (subscriptionToken != "+" && topicToken != subscriptionToken)
        {
            ESP_LOGV(TAG, "Tokens do not match and subscription token is not + --> NO MATCH");
            return false;
        }

        ESP_LOGV(TAG, "Current token index: topic: %d, subscription: %d", topicIndex, subscriptionIndex);
        if (topicIndex == lastTopicIndex + 1 || subscriptionIndex == lastSubscriptionIndex + 1)
        {
            ESP_LOGV(TAG, "End of tokens. Topic: %s, Subscription: %s", topicStr.substring(topicIndex).c_str(), subscriptionStr.substring(subscriptionIndex).c_str());
            bool match = ((subscriptionToken == "+" && topicIndex == lastTopicIndex + 1) || topicStr.substring(topicIndex) == subscriptionStr.substring(subscriptionIndex));
            ESP_LOGV(TAG, "End of tokens. Match: %s", match ? "true" : "false");
            return match;
        }

        topicIndex = topicStr.indexOf('/', topicIndex) + 1;
        subscriptionIndex = subscriptionStr.indexOf('/', subscriptionIndex) + 1;
        ESP_LOGV(TAG, "Next token index: topic: %d, subscription: %d", topicIndex, subscriptionIndex);
        topicToken = topicStr.substring(topicIndex, topicStr.indexOf('/', topicIndex));
        subscriptionToken = subscriptionStr.substring(subscriptionIndex, subscriptionStr.indexOf('/', subscriptionIndex));

        ESP_LOGV(TAG, "Next topic token: %s, subscription token: %s", topicToken.c_str(), subscriptionToken.c_str());
    }

    return false;
}
