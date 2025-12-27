#include "GitHubReleaseUpdate.h"

#include <StreamString.h>
#include <esp_ota_ops.h>
#include <esp_partition.h>
#include "Logger.h"

/**
 * @brief Enhanced update method with retry logic for GitHub
 *
 * @param client TCP client instance used for the HTTP connection
 * @param url Full URL pointing to the firmware binary
 * @param maxRetries Maximum number of retry attempts
 * @return HttpUpdateResult
 */
HttpUpdateResult GitHubReleaseUpdate::update(WiFiClient& client, const String& url, int maxRetries) {
    return updateWithRetry(client, url, maxRetries);
}

/**
 * @brief GitHub-specific update with better error handling
 *
 * @param firmwareFile Firmware filename (e.g., "esp32.bin")
 * @param isPrerelease Whether to fetch prerelease versions
 * @return HttpUpdateResult
 */
HttpUpdateResult GitHubReleaseUpdate::updateFromGitHub(const String& firmwareFile, bool isPrerelease) {
    String baseUrl = "https://github.com/ESPresense/ESPresense/releases/latest/download/" + firmwareFile;

    if (isPrerelease) {
#ifdef BRANCH
        baseUrl = "https://espresense.com/artifacts/latest/download/" BRANCH "/" + firmwareFile;
#else
        baseUrl = "https://espresense.com/releases/latest-any/download/" + firmwareFile;
#endif
    }

    WiFiClientSecure client;
    if (!setupGitHubClient(client)) {
        _setLastError(HTTP_UE_SSL_HANDSHAKE_FAILED);
        return HTTP_UPDATE_FAILED;
    }

    return updateWithRetry(client, baseUrl, _maxRetries);
}

/**
 * @brief Enhanced client setup for GitHub with proper SSL settings
 */
bool GitHubReleaseUpdate::setupGitHubClient(WiFiClientSecure& client) {
    // Configure for GitHub's SSL requirements
    client.setInsecure();  // Allow GitHub's certificates
    client.setHandshakeTimeout(15);  // Increased for GitHub
    client.setTimeout(20);  // Increased timeout

    // Set proper User-Agent to avoid blocking
    // This needs to be set in HTTPClient, not WiFiClientSecure

    return true;
}

/**
 * @brief Test GitHub connectivity before attempting update
 */
bool GitHubReleaseUpdate::testGitHubConnectivity(const String& url) {
    WiFiClientSecure testClient;
    if (!setupGitHubClient(testClient)) {
        return false;
    }

    HTTPClient http;
    http.useHTTP10(true);
    http.setTimeout(10000);  // Shorter timeout for connectivity test

    if (!http.begin(testClient, url)) {
        return false;
    }

    // Send HEAD request to test connectivity
    int httpCode = http.sendRequest("HEAD");
    http.end();

    // Check for GitHub rate limiting
    if (httpCode == 403) {
        Log.println("GitHub rate limit detected, backing off...");
        _setLastError(HTTP_UE_GITHUB_RATE_LIMIT);
        return false;
    }

    return httpCode > 0;
}

/**
 * @brief Retry logic with exponential backoff for GitHub
 */
HttpUpdateResult GitHubReleaseUpdate::updateWithRetry(WiFiClient& client, const String& url, int maxRetries) {
    HttpUpdateResult lastResult = HTTP_UPDATE_FAILED;

    for (int attempt = 0; attempt < maxRetries; attempt++) {
        Log.printf("Update attempt %d/%d for URL: %s\n", attempt + 1, maxRetries, url.c_str());

        // Rate limiting for GitHub
        if (isGitHubUrl(url) && _requestCount > 0) {
            unsigned long timeSinceLastRequest = millis() - _lastRequestTime;
            if (timeSinceLastRequest < 2000) {  // 2 second delay between requests
                Log.printf("Rate limiting: waiting %dms\n", 2000 - timeSinceLastRequest);
                delay(2000 - timeSinceLastRequest);
            }
        }

        // Test connectivity first for GitHub URLs
        if (isGitHubUrl(url) && !testGitHubConnectivity(url)) {
            Log.printf("Connectivity test failed for attempt %d\n", attempt + 1);
            if (attempt < maxRetries - 1) {
                delay(5000 * (attempt + 1));  // Progressive delay
                continue;
            }
            return HTTP_UPDATE_FAILED;
        }

        // Perform the update
        HTTPClient http;
        http.useHTTP10(true);
        http.setTimeout(_httpClientTimeout);
        http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);

        // Set User-Agent for GitHub
        http.setUserAgent(_githubUserAgent);

        if (!http.begin(client, url)) {
            Log.printf("HTTP client begin failed for attempt %d\n", attempt + 1);
            if (attempt < maxRetries - 1) {
                delay(3000 * (attempt + 1));
                continue;
            }
            _setLastError(HTTP_UE_CONNECTION_TIMEOUT);
            return HTTP_UPDATE_FAILED;
        }

        _requestCount++;
        _lastRequestTime = millis();

        lastResult = handleUpdate(http);
        http.end();

        if (lastResult == HTTP_UPDATE_OK) {
            Log.printf("Update successful on attempt %d\n", attempt + 1);
            return HTTP_UPDATE_OK;
        }

        Log.printf("Update attempt %d failed: %s\n", attempt + 1, getLastErrorString().c_str());

        if (attempt < maxRetries - 1) {
            // Exponential backoff with jitter
            int baseDelay = 5000 * (1 << attempt);  // 5s, 10s, 20s
            int jitter = random(1000);  // Add up to 1s jitter
            delay(baseDelay + jitter);
        }
    }

    Log.printf("All %d update attempts failed\n", maxRetries);
    return lastResult;
}

/**
 * @brief Enhanced update handling with better GitHub error detection
 */
HttpUpdateResult GitHubReleaseUpdate::handleUpdate(HTTPClient& http) {
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

    // Handle GitHub-specific responses
    if (isGitHubUrl(http.getLocation())) {
        if (code == 403) {
            // Check for rate limiting
            String rateLimitRemaining = http.header("X-RateLimit-Remaining");
            if (rateLimitRemaining.length() > 0 && rateLimitRemaining.toInt() == 0) {
                Log.println("GitHub API rate limit exceeded");
                _setLastError(HTTP_UE_GITHUB_RATE_LIMIT);
                goto exit;
            }
        }

        if (code == 429) {
            Log.println("GitHub rate limiting active");
            _setLastError(HTTP_UE_GITHUB_RATE_LIMIT);
            goto exit;
        }
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
 * @brief Check if URL is a GitHub URL
 */
bool GitHubReleaseUpdate::isGitHubUrl(const String& url) {
    return url.indexOf("github.com") >= 0 || url.indexOf("espresense.com") >= 0;
}

/**
 * @brief Run update with enhanced error handling
 */
bool GitHubReleaseUpdate::runUpdate(Stream& in, uint32_t size) {
    StreamString error;

    if (_cbProgress) {
        Update.onProgress(_cbProgress);
    }

    if (!Update.begin(size, U_FLASH, _ledPin, _ledOn)) {
        _lastError = Update.getError();
        Update.printError(error);
        error.trim();
        Log.printf("Update.begin failed! (%s)\r\n", error.c_str());
        return false;
    }

    if (_cbProgress) {
        _cbProgress(0, size);
    }

    size_t written = Update.writeStream(in);
    if (written != size) {
        _lastError = Update.getError();
        Update.printError(error);
        error.trim();
        Log.printf("Update.writeStream failed! Expected %d, got %d (%s)\r\n", size, written, error.c_str());
        return false;
    }

    if (_cbProgress) {
        _cbProgress(size, size);
    }

    if (!Update.end()) {
        _lastError = Update.getError();
        Update.printError(error);
        error.trim();
        Log.printf("Update.end failed! (%s)\r\n", error.c_str());
        return false;
    }

    return true;
}

/**
 * @brief Get last error code
 */
int GitHubReleaseUpdate::getLastError(void) {
    return _lastError;
}

/**
 * @brief Get last error string with enhanced GitHub error messages
 */
String GitHubReleaseUpdate::getLastErrorString(void) {
    if (_lastError == 0) {
        return String();  // no error
    }

    // error from Update class
    if (_lastError > 0) {
        StreamString error;
        Update.printError(error);
        error.trim();
        return String("Update error: ") + error;
    }

    // error from http client
    if (_lastError > -100) {
        return String("HTTP error: ") + HTTPClient::errorToString(_lastError);
    }

    switch (_lastError) {
        case HTTP_UE_TOO_LESS_SPACE:
            return "Not Enough Space";
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
