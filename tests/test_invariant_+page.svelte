#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

#define BASE_URL "http://localhost:8080"

static long send_request(const char *endpoint, const char *csrf_header)
{
    CURL *curl = curl_easy_init();
    long http_code = 0;

    if (curl) {
        char url[256];
        snprintf(url, sizeof(url), "%s%s", BASE_URL, endpoint);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "ssid=test&password=test");
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);

        struct curl_slist *headers = NULL;
        if (csrf_header && strlen(csrf_header) > 0)
            headers = curl_slist_append(headers, csrf_header);
        if (headers)
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed for %s: %s\n",
                    endpoint, curl_easy_strerror(res));
            http_code = -1;
        } else {
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        }

        if (headers) curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    return http_code;
}

START_TEST(test_endpoints_reject_requests_without_csrf_header)
{
    /* Invariant: Endpoints must reject requests that lack the X-Requested-With
     * header.  Browsers enforce CORS preflight for custom headers, so this
     * header cannot be set by a cross-origin page — stopping browser-based
     * CSRF attacks (the primary threat for a local-network device). */
    const char *endpoints[] = { "/wifi/main", "/restart" };

    for (int e = 0; e < 2; e++) {
        long code = send_request(endpoints[e], "");
        ck_assert_msg(code != -1,
            "Network error: could not connect to server for endpoint %s",
            endpoints[e]);
        ck_assert_msg(code == 403,
            "Endpoint %s accepted a request without X-Requested-With header (code=%ld)",
            endpoints[e], code);
    }
}
END_TEST

START_TEST(test_endpoints_accept_requests_with_csrf_header)
{
    /* Invariant: Requests from the legitimate web UI (which always include
     * X-Requested-With: XMLHttpRequest) must be accepted. */
    const char *endpoints[] = { "/wifi/main", "/restart" };
    const char *csrf_header = "X-Requested-With: XMLHttpRequest";

    for (int e = 0; e < 2; e++) {
        long code = send_request(endpoints[e], csrf_header);
        ck_assert_msg(code != -1,
            "Network error: could not connect to server for endpoint %s",
            endpoints[e]);
        ck_assert_msg(code >= 200 && code < 300,
            "Endpoint %s rejected a legitimate UI request with X-Requested-With header (code=%ld)",
            endpoints[e], code);
    }
}
END_TEST

Suite *security_suite(void)
{
    Suite *s = suite_create("Security");
    TCase *tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_endpoints_reject_requests_without_csrf_header);
    tcase_add_test(tc_core, test_endpoints_accept_requests_with_csrf_header);
    suite_add_tcase(s, tc_core);
    return s;
}

int main(void)
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    Suite *s = security_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_NORMAL);
    int failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    curl_global_cleanup();
    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
