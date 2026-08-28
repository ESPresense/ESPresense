#pragma once

#include <ArduinoJson.h>

#include <cstddef>
#include <cstdint>

// Body builder for /json/tele, kept apart from HttpWebServer.cpp so it can be tested
// without the web server. See serveTele() for why this endpoint avoids the heap.
//
// Returns bytes written, or 0 if the document did not fit — callers must refuse rather
// than send a partial body. A 200 carrying half a JSON object is the "success that lies"
// failure mode this endpoint exists to help diagnose, so it must not be one.
//
// `room` is user-set free text (main.cpp: HeadlessWiFiSettings.string("room", ...)), so a
// quote, backslash or control character in it would break a hand-formatted body.
// ArduinoJson escapes at serialization time; that is the whole reason not to snprintf this.
inline size_t buildTeleJson(char *out, size_t size, const char *room,
                            uint32_t freeHeap, uint32_t maxHeap, uint32_t fingerprints) {
    if (out == nullptr || size == 0) return 0;

    // Stack, not DynamicJsonDocument: this endpoint has to answer when the heap is too
    // short for /json to reply at all. A const char* is stored by pointer rather than
    // copied, so a long room costs no capacity here — it shows up in the length check
    // below instead, which is what catches it.
    StaticJsonDocument<JSON_OBJECT_SIZE(4)> doc;
    doc["room"] = room;
    doc["freeHeap"] = freeHeap;
    doc["maxHeap"] = maxHeap;
    doc["fingerprints"] = fingerprints;

    if (doc.overflowed()) return 0;
    if (measureJson(doc) >= size) return 0;  // >= : leave room for the NUL
    return serializeJson(doc, out, size);
}
