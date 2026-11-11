#!/usr/bin/env python3
"""Read-only, bounded MQTT capture of personal-tag BLE reports for replay."""
import argparse
import asyncio
import json
import os
import queue
import time
import uuid

import paho.mqtt.client as mqtt
from collect import Recorder


async def capture(args):
    recorder = Recorder(args)
    pending = queue.Queue(maxsize=4096)
    stats = {"received": 0, "selected": 0, "queue_drops": 0, "connected": False}
    prefixes = tuple(args.prefix)

    def enqueue(source, data):
        try:
            pending.put_nowait((source, data))
        except queue.Full:
            stats["queue_drops"] += 1

    def connected(client, userdata, flags, reason, properties):
        stats["connected"] = reason == 0
        enqueue("mqtt_connection", {"connected": stats["connected"], "reason": str(reason)})
        if reason == 0:
            # Subscribe only. No availability, discovery, control, or other
            # application message is ever published by this recorder.
            client.subscribe("espresense/devices/#", qos=0)

    def disconnected(client, userdata, flags, reason, properties):
        stats["connected"] = False
        enqueue("mqtt_connection", {"connected": False, "reason": str(reason)})

    def message(client, userdata, msg):
        stats["received"] += 1
        parts = msg.topic.split("/")
        if len(parts) != 4 or parts[:2] != ["espresense", "devices"] or not parts[2].startswith(prefixes):
            return
        stats["selected"] += 1
        try:
            data = json.loads(msg.payload)
        except (ValueError, UnicodeError):
            data = {"unparsed": msg.payload.decode("utf-8", "replace")}
        enqueue("ble_report", {"topic": msg.topic, "retained": msg.retain, "payload": data})

    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2,
                         client_id="roomval-" + uuid.uuid4().hex[:12], protocol=mqtt.MQTTv311)
    client.on_connect = connected
    client.on_disconnect = disconnected
    client.on_message = message
    client.reconnect_delay_set(min_delay=2, max_delay=30)
    client.connect_async(args.host, args.port, keepalive=60)
    client.loop_start()
    last_status = 0
    try:
        while recorder.active():
            for _ in range(4096):
                try:
                    source, data = pending.get_nowait()
                except queue.Empty:
                    break
                recorder.record("ble", source, data)
            if time.monotonic() - last_status >= 5:
                recorder.record("ble", "mqtt_stats", dict(stats))
                recorder.status()
                last_status = time.monotonic()
            await asyncio.sleep(min(0.1, max(0, args.until - time.time())))
    except BaseException:
        recorder.stop_reason = "failed"
        raise
    finally:
        client.disconnect()
        client.loop_stop()
        recorder.status(done=True)
        for _, writer, path in recorder.writers.values():
            writer.close()
            path.rename(path.with_suffix(""))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--host", required=True, help="Existing configured anonymous MQTT broker")
    parser.add_argument("--port", type=int, default=1883)
    parser.add_argument("--prefix", action="append",
                        help="Tag ID prefixes to retain; defaults to phone/watch/irk/keys/wallet")
    parser.add_argument("--until", type=float, required=True)
    parser.add_argument("--out", required=True)
    parser.add_argument("--max-bytes", type=int, default=512 * 1024**2)
    args = parser.parse_args()
    args.prefix = args.prefix or ["phone:", "watch:", "irk:", "keys:", "wallet:"]
    if not time.time() < args.until <= time.time() + 48 * 3600:
        parser.error("Deadline must be in the next 48 hours")
    os.umask(0o077)
    asyncio.run(capture(args))


if __name__ == "__main__":
    main()
