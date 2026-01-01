#include "HttpReleaseUpdate.h"

#include <StreamString.h>
#include <esp_ota_ops.h>
#include <esp_partition.h>
#include "Logger.h"

/**
 * @brief Performs an OTA update by fetching the firmware from the given URL using the provided WiFi client.
 *
 * @param client TCP client instance used for the HTTP connection.
 * @param url Full URL pointing to the firmware binary.
 * @return HttpUpdateResult `HTTP_UPDATE_OK` when the update completes successfully, `HTTP_UPDATE_NO_UPDATES` if the server indicates no new firmware, or another `HttpUpdateResult` value representing the failure reason.
 */
HttpUpdateResult HttpReleaseUpdate::update(WiFiClient& client, const String& url, int maxRetries) {
    HTTPClient http;
    http.useHTTP10(true);
    http.setTimeout(_httpClientTimeout);
    http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
    if (!http.begin(client, url))
        return HTTP_UPDATE_FAILED;
    return handleUpdate(http);
}

/**
 * @brief Enhanced update method with retry logic for GitHub
 *
 * @param client TCP client instance used for the HTTP connection
 * @param url Full URL pointing to the firmware binary
 * @param maxRetries Maximum number of retry attempts
 * @return HttpUpdateResult
 */
HttpUpdateResult HttpReleaseUpdate::updateWithRetry(WiFiClient& client, const String& url, int maxRetries) {
    HttpUpdateResult lastResult = HTTP_UPDATE_FAILED;

    for (int attempt = 0; attempt < maxRetries; attempt++) {
        Log.printf("Update attempt %d/%d for URL: %s\n", attempt + 1, maxRetries, url.c_str());

        // Exponential backoff with jitter
        if (attempt > 0) {
            int baseDelay = 5000 * (1 << (attempt - 1));  // 5s, 10s, 20s
            int jitter = random(1000);  // Add up to 1s jitter
            delay(baseDelay + jitter);
        }

        HTTPClient http;
        http.useHTTP10(true);
        http.setTimeout(_httpClientTimeout);
        http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
        if (!http.begin(client, url)) {
            Log.printf("HTTP client begin failed for attempt %d\n", attempt + 1);
            continue;
        }

        lastResult = handleUpdate(http);
        http.end();

        if (lastResult == HTTP_UPDATE_OK) {
            Log.printf("Update successful on attempt %d\n", attempt + 1);
            return HTTP_UPDATE_OK;
        }

        Log.printf("Update attempt %d failed: %s\n", attempt + 1, getLastErrorString().c_str());
    }

    Log.printf("All %d update attempts failed\n", maxRetries);
    return HTTP_UPDATE_FAILED;
}

/**
 * return error code as int
 * @return int error code
 */
int HttpReleaseUpdate::getLastError(void) {
    return _lastError;
}

/**
 * return error code as String
 * @return String error
 */
String HttpReleaseUpdate::getLastErrorString(void) {
    if (_lastError == 0) {
        return String();  // no error
    }

    // error from Update class
    if (_lastError > 0) {
        StreamString error;
        Update.printError(error);
        error.trim();  // remove line ending
        return String("Update error: ") + error;
    }

    // error from http client
    if (_lastError > -100) {
        return String("HTTP error: ") + HTTPClient::errorToString(_lastError);
    }

    switch (_lastError) {
        case HTTP_UE_TOO_LESS_SPACE:
            return "Not Enough space";
        case HTTP_UE_SERVER_NOT_REPORT_SIZE:
            return "Server Did Not Report Size";
        case HTTP_UE_SERVER_FILE_NOT_FOUND:
            return "File Not Found (404)";
        case HTTP_UE_SERVER_FORBIDDEN:
            return "Forbidden (403)";
        case HTTP_UE_SERVER_WRONG_HTTP_CODE:
            return "Wrong HTTP Code";
        case HTTP_UE_SERVER_FAULTY_MD5:
            return "Wrong MD5";
        case HTTP_UE_BIN_VERIFY_HEADER_FAILED:
            return "Verify Bin Header Failed";
        case HTTP_UE_BIN_FOR_WRONG_FLASH:
            return "New Binary Does Not Fit Flash Size";
        case HTTP_UE_NO_PARTITION:
            return "Partition Could Not be Found";
        case HTTP_UE_GITHUB_RATE_LIMIT:
            return "GitHub Rate Limit Exceeded - Try Again Later";
        case HTTP_UE_SSL_HANDSHAKE_FAILED:
            return "SSL/TLS Handshake Failed";
        case HTTP_UE_CONNECTION_TIMEOUT:
            return "Connection Timeout";
    }

    return String();
}

/**
 * @brief Performs an HTTP-based firmware update using the provided HTTP client.
 *
 * Initiates an HTTP GET via the given client, interprets the response, and if a valid firmware
 * payload is available writes it to flash. The function updates the internal error state,
 * invokes start/progress/end callbacks when configured, and ends the HTTP session before returning.
 * It may trigger a device restart if the update completes and reboot-on-update is enabled.
 *
 * @param http HTTP client already configured for the update request.
 * @return HttpUpdateResult
 *         - `HTTP_UPDATE_OK` if the firmware was written successfully.
 *         - `HTTP_UPDATE_NO_UPDATES` if the server responded with "Not Modified".
 *         - `HTTP_UPDATE_FAILED` for any failure (HTTP errors, missing/oversized payload, write/verify errors, etc.).
 */
HttpUpdateResult HttpReleaseUpdate::handleUpdate(HTTPClient& http) {
    HttpUpdateResult ret = HTTP_UPDATE_FAILED;
#ifdef VERSION
    http.setUserAgent("ESPresense/" VERSION);
#else
    http.setUserAgent("ESPresense/0.0");
#endif

    int code = http.GET();
    if (code <= 0) {
        Log.printf("HTTP error: %s\r\n", http.errorToString(code).c_str());
        _lastError = code;
        goto exit;
    }

    switch (code) {
        case HTTP_CODE_OK: {
            int len = http.getSize();
            if (len > 0) {
                int sketchFreeSpace = ESP.getFreeSketchSpace();
                if (!sketchFreeSpace) {
                    _lastError = HTTP_UE_NO_PARTITION;
                    goto exit;
                }

                if (len > sketchFreeSpace) {
                    Log.printf("FreeSketchSpace too low (%d) needed: %d\r\n", sketchFreeSpace, len);
                    _lastError = HTTP_UE_TOO_LESS_SPACE;
                    goto exit;
                }

                if (_cbStart) {
                    _cbStart();
                }

                WiFiClient* tcp = http.getStreamPtr();
                if (runUpdate(*tcp, len)) {
                    ret = HTTP_UPDATE_OK;
                    if (_cbEnd) {
                        _cbEnd(true);
                    }

                    if (_rebootOnUpdate) {
                        Log.println("Update complete, rebooting...");
                        ESP.restart();
                    }
                } else {
                    ret = HTTP_UPDATE_FAILED;

                    if (_cbEnd) {
                        _cbEnd(false);
                    }
                }
            } else {
                _lastError = HTTP_UE_SERVER_NOT_REPORT_SIZE;
                Log.printf("Content-Length was 0 or wasn't set by Server?!\r\n");
                goto exit;
            }
        } break;
        case HTTP_CODE_NOT_MODIFIED:
            ret = HTTP_UPDATE_NO_UPDATES;
            break;
        case HTTP_CODE_NOT_FOUND:
            _lastError = HTTP_UE_SERVER_FILE_NOT_FOUND;
            ret = HTTP_UPDATE_FAILED;
            break;
        case HTTP_CODE_FORBIDDEN:
            _lastError = HTTP_UE_SERVER_FORBIDDEN;
            ret = HTTP_UPDATE_FAILED;
            break;
        default:
            _lastError = HTTP_UE_SERVER_WRONG_HTTP_CODE;
            Log.printf("HTTP Code is (%d)\r\n", code);
            break;
    }

exit:
    http.end();
    return ret;
}

/**
 * @brief Writes a firmware image from a stream into flash and finalizes the OTA update.
 *
 * Performs the OTA update sequence: initializes the updater for the provided size, streams exactly
 * `size` bytes from `in` into flash, and finalizes the update. If a progress callback is set it
 * will be invoked at the start and completion of the write.
 *
 * @param in Input stream that provides the firmware image bytes; exactly `size` bytes are expected.
 * @param size Expected number of bytes to write to flash.
 * @return true if the update was written and finalized successfully, false on any failure.
 *
 * On failure, the instance's internal last-error code is updated to the updater error.
 */
bool HttpReleaseUpdate::runUpdate(Stream& in, uint32_t size) {
    StreamString error;

    if (_cbProgress) {
        Update.onProgress(_cbProgress);
    }

    if (!Update.begin(size, U_FLASH, _ledPin, _ledOn)) {
        _lastError = Update.getError();
        Update.printError(error);
        error.trim();  // remove line ending
        Log.printf("Update.begin failed! (%s)\r\n", error.c_str());
        return false;
    }

    if (_cbProgress) {
        _cbProgress(0, size);
    }

    if (Update.writeStream(in) != size) {
        _lastError = Update.getError();
        Update.printError(error);
        error.trim();  // remove line ending
        Log.printf("Update.writeStream failed! (%s)\r\n", error.c_str());
        return false;
    }

    if (_cbProgress) {
        _cbProgress(size, size);
    }

    if (!Update.end()) {
        _lastError = Update.getError();
        Update.printError(error);
        error.trim();  // remove line ending
        Log.printf("Update.end failed! (%s)\r\n", error.c_str());
        return false;
    }

    return true;
}
