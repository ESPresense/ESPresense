#pragma once

#include <cstddef>
#include <string>

// Whether a POST to /wifi/main may be applied, kept apart from Settings.cpp so it can be
// tested without the web server or SPIFFS.
//
// The configuration AP protects the setup portal. Enabling it with no usable password would
// leave an open AP while the UI claimed otherwise, so that combination is rejected outright
// rather than silently stored. A too-short password is rejected too: WPA2-PSK requires 8-63
// bytes, and esp_wifi_set_config() would fail the AP with anything else.
//
// `submittedPassword` is the raw form value. When the UI echoed back the stored password it
// sends the masked placeholder instead of the real one; Param::set drops that value rather
// than storing it, so it means "unchanged" and needs no length check here.
//
// `hasStoredPassword` is whether /ap-password already exists on flash, so a masked resubmit
// can still enable protection when a password was saved earlier.
namespace Settings {

constexpr std::size_t minimumApPasswordLength = 8;
constexpr std::size_t maximumApPasswordLength = 63;

// Value the GET response returns in place of a stored password. POSTing it back means
// "leave the saved password alone".
constexpr const char* maskedPassword = "***###***";

inline bool apPasswordAcceptable(bool enabled, const std::string& submittedPassword, bool hasStoredPassword) {
    const bool unchanged = submittedPassword == maskedPassword;
    const bool badLength = !unchanged && !submittedPassword.empty() &&
        (submittedPassword.size() < minimumApPasswordLength ||
         submittedPassword.size() > maximumApPasswordLength);
    if (badLength) return false;
    // Enabling protection needs a usable password: the one submitted, or one already saved.
    if (enabled && (unchanged || submittedPassword.empty()) && !hasStoredPassword) return false;
    return true;
}

}  // namespace Settings
