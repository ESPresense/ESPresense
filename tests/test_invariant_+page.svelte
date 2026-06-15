#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

#define BASE_URL "http://localhost:8080"

static long send_request(const char *endpoint, const char *auth_header)
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
        if (auth_header && strlen(auth_header) > 0) {
            headers = curl_slist_append(headers, auth_header);
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        }
        
        curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        
        if (headers) curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    return http_code;
}

START_TEST(test_protected_endpoints_reject_unauthenticated)
{
    // Invariant: Protected endpoints must reject requests without valid authentication
    const char *endpoints[] = { "/wifi/main", "/restart" };
    const char *auth_cases[] = {
        "",                                          // No auth (exact exploit case)
        "Authorization: Bearer expired_token_123",   // Expired/invalid token
        "Authorization: Bearer !!!malformed!!!"      // Malformed token
    };
    
    for (int e = 0; e < 2; e++) {
        for (int a = 0; a < 3; a++) {
            long code = send_request(endpoints[e], auth_cases[a]);
            ck_assert_msg(code == 401 || code == 403,
                "Endpoint %s accepted unauthenticated request (auth='%s', code=%ld)",
                endpoints[e], auth_cases[a], code);
        }
    }
}
END_TEST

Suite *security_suite(void)
{
    Suite *s = suite_create("Security");
    TCase *tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_protected_endpoints_reject_unauthenticated);
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