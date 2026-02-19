<script lang="ts">
    import { onMount, onDestroy } from "svelte";
    import { mainSettings } from "$lib/stores";
    import type { MainSettings } from "$lib/types";

    let s = $state(false);
    let wifiNetworks: Record<string, number> = $state({});
    let isScanning = $state(false);
    let isDestroyed = false;

    async function fetchWifiNetworks() {
        if (isScanning || isDestroyed) return; // Prevent concurrent scans and post-destroy scans
        try {
            isScanning = true;
            const response = await fetch("/wifi/scan");
            const data = await response.json();
            if (data.networks) {
                wifiNetworks = data.networks;
            } else {
                console.error("Unexpected response format:", data);
            }
        } catch (error) {
            console.error("Failed to fetch Wi-Fi networks:", error);
        } finally {
            isScanning = false;
            // Schedule next scan after 5 seconds
            if (!isDestroyed) {
                setTimeout(fetchWifiNetworks, 5000);
            }
        }
    }

    function startScanCycle() {
        requestAnimationFrame(fetchWifiNetworks);
    }

    onMount(() => {
        startScanCycle();
    });

    onDestroy(() => {
        isDestroyed = true;
    });

    async function handleSubmit(event: SubmitEvent) {
        try {
            event.preventDefault();
            s = true;
            const form = event.target as HTMLFormElement;
            const formData = new FormData(form);
            const params = new URLSearchParams();
            for (const [key, value] of formData.entries()) {
                if (typeof value === "string") {
                    params.append(key, value);
                }
            }
            await fetch("/wifi/main", { method: "POST", body: params });

            try {
                await fetch("/restart", { method: "POST", signal: AbortSignal.timeout(1000)});
            } catch (error) {
                // often the restart request will fail, so we ignore the error
            }

            // Reload settings after save, with retries
            let retries = 5;
            let data;
            while (retries > 0) {
                try {
                    const response = await fetch("/wifi/main");
                    data = await response.json();
                    mainSettings.set(data);
                    break; // Exit loop on success
                } catch (error) {
                    retries--;
                    if (retries === 0) {
                        console.error("Failed to reload settings after multiple retries:", error);
                        // Optionally display an error message to the user
                        break;
                    }
                    // Wait for 1 second before retrying
                    await new Promise((resolve) => setTimeout(resolve, 1000));
                }
            }
        } finally {
            s = false;
        }
    }

    // Make sure your store's data keys match the names below.
    const mainSettingsData = $derived($mainSettings as MainSettings);
</script>

<div class="bg-gray-100 dark:bg-gray-800 rounded-lg shadow p-6">
    {#if mainSettingsData != null && $mainSettings?.defaults != null && $mainSettings?.values != null}
        <form onsubmit={handleSubmit} class="space-y-6">
            <!-- Room Configuration -->
            <h2 class="text-xl font-semibold">Room Configuration</h2>
            <div>
                <label for="room-name" class="block text-sm font-medium">Room Name</label>
                <input id="room-name" type="text" name="room" bind:value={$mainSettings.values.room} placeholder={$mainSettings.defaults.room} class="mt-1 block w-full rounded-md" />
            </div>

            <!-- WiFi Configuration -->
            <h2 class="text-xl font-semibold">WiFi Configuration</h2>

            <div class="mb-2">
                <label for="wifi-ssid" class="block text-sm font-medium mb-1">WiFi SSID</label>
                <input
                    id="wifi-ssid"
                    type="text"
                    name="wifi-ssid"
                    bind:value={$mainSettings.values["wifi-ssid"]}
                    placeholder="Enter WiFi SSID"
                    class="block w-full rounded-xl px-4 py-3 text-base"
                />
            </div>

            <div>
                <div class="bg-gray-50 dark:bg-gray-900 rounded-xl shadow-sm border border-gray-200 dark:border-gray-600">
                    <div class="px-4 py-3 flex items-center justify-between border-b border-gray-200 dark:border-gray-700">
                        <div class="text-base font-medium dark:text-white">Available Networks</div>
                        {#if isScanning}
                            <div class="ios-spinner"></div>
                        {/if}
                    </div>
                    <ul class="divide-y divide-gray-200 dark:divide-gray-700">
                        {#each Object.entries(wifiNetworks) as [ssid, rssi]}
                            <li>
                                <button
                                    type="button"
                                    onclick={() => {$mainSettings.values["wifi-ssid"] = ssid}}
                                    class="w-full flex items-center cursor-pointer px-4 py-3 hover:bg-gray-100 dark:hover:bg-gray-800 transition-colors text-left border-0 bg-transparent"
                                >
                                <div class="grow">
                                    <div class="text-base dark:text-white">{ssid}</div>
                                </div>
                                <div class="flex items-center space-x-2">
                                    <div class="flex" title={`Signal Strength: ${rssi} dBm`}>
                                        {#each Array(3) as _, i}
                                            <div
                                                class={`w-1.5 h-3 mx-0.5 rounded-sm transform origin-bottom ${
                                                    Math.abs(rssi) < (90 - i * 15)
                                                    ? 'bg-blue-500 dark:bg-blue-400'
                                                    : 'bg-gray-300 dark:bg-gray-600'
                                                }`}
                                                style={`transform: scale(1, ${0.6 + i * 0.2})`}
                                            ></div>
                                        {/each}
                                    </div>
                                </div>
                                </button>
                            </li>
                        {/each}
                    </ul>
                </div>
            </div>

            <div>
                <label for="wifi-password" class="block text-sm font-medium">WiFi Password</label>
                <input id="wifi-password" type="password" name="wifi-password" bind:value={$mainSettings.values["wifi-password"]} placeholder="Enter WiFi Password" class="mt-1 block w-full rounded-md" />
            </div>

            <div>
                <label for="wifi-timeout" class="block text-sm font-medium"> Seconds to wait for WiFi before captive portal (-1 = forever) </label>
                <input id="wifi-timeout" type="number" name="wifi_timeout" bind:value={$mainSettings.values.wifi_timeout} placeholder={String($mainSettings.defaults.wifi_timeout)} step="1" min="-2147483648" max="2147483647" class="mt-1 block w-full rounded-md" />
            </div>

            <label for="wifi-sleep" class="flex items-center space-x-2">
                <input id="wifi-sleep" type="checkbox" name="wifi_sleep" value="1" bind:checked={$mainSettings.values.wifi_sleep} class="h-4 w-4 rounded border-gray-300 text-blue-600 focus:ring-blue-500" />
                <span>Enable WiFi modem sleep mode</span>
            </label>

            <div>
                <label for="portal-timeout" class="block text-sm font-medium"> Seconds to wait in captive portal before rebooting </label>
                <input id="portal-timeout" type="number" name="portal_timeout" bind:value={$mainSettings.values.portal_timeout} placeholder={String($mainSettings.defaults.portal_timeout)} step="1" min="-2147483648" max="2147483647" class="mt-1 block w-full rounded-md" />
            </div>

            <div>
                <label for="ethernet-type" class="block text-sm font-medium">Ethernet Type</label>
                <select id="ethernet-type" name="eth" bind:value={$mainSettings.values.eth} class="mt-1 block w-full rounded-md">
                    <option value="0">None</option>
                    <option value="1">WT32-ETH01</option>
                    <option value="2">ESP32-POE</option>
                    <option value="3">WESP32</option>
                    <option value="4">QuinLED-ESP32</option>
                    <option value="5">TwilightLord-ESP32</option>
                    <option value="6">ESP32Deux</option>
                    <option value="7">KIT-VE</option>
                    <option value="8">LilyGO-T-ETH-POE</option>
                    <option value="9">GL-inet GL-S10 v2.1 Ethernet</option>
                    <option value="10">EST-PoE-32</option>
                    <option value="11">LilyGO-T-ETH-Lite (RTL8201)</option>
                    <option value="12">ESP32-POE_A1</option>
                </select>
            </div>

            <!-- MQTT Configuration -->
            <h2 class="text-xl font-semibold">MQTT Configuration</h2>
            <div>
                <label for="mqtt-host" class="block text-sm font-medium">Server</label>
                <input id="mqtt-host" type="text" name="mqtt_host" bind:value={$mainSettings.values.mqtt_host} placeholder={$mainSettings.defaults.mqtt_host} class="mt-1 block w-full rounded-md" />
            </div>

            <div>
                <label for="mqtt-port" class="block text-sm font-medium">Port</label>
                <input id="mqtt-port" type="number" name="mqtt_port" bind:value={$mainSettings.values.mqtt_port} placeholder={String($mainSettings.defaults.mqtt_port)} step="1" min="-2147483648" max="2147483647" class="mt-1 block w-full rounded-md" />
            </div>

            <div>
                <label for="mqtt-user" class="block text-sm font-medium">Username</label>
                <!-- Using a password input as in the legacy page -->
                <input id="mqtt-user" type="password" name="mqtt_user" bind:value={$mainSettings.values.mqtt_user} class="mt-1 block w-full rounded-md" />
            </div>

            <div>
                <label for="mqtt-pass" class="block text-sm font-medium">Password</label>
                <input id="mqtt-pass" type="password" name="mqtt_pass" bind:value={$mainSettings.values.mqtt_pass} class="mt-1 block w-full rounded-md" />
            </div>

            <div class="space-y-4">
                <label for="discovery" class="flex items-center space-x-2">
                    <input id="discovery" type="checkbox" name="discovery" value="1" bind:checked={$mainSettings.values.discovery} class="h-4 w-4 rounded border-gray-300 text-blue-600 focus:ring-blue-500" />
                    <span>Send to discovery topic</span>
                </label>

                <div class="ml-6">
                    <label for="discovery-prefix" class="block text-sm font-medium"> Home Assistant discovery topic prefix </label>
                    <input id="discovery-prefix" type="text" name="discovery_prefix" bind:value={$mainSettings.values.discovery_prefix} placeholder={$mainSettings.defaults.discovery_prefix} class="mt-1 block w-full rounded-md" />
                </div>

                <label for="pub-tele" class="flex items-center space-x-2">
                    <input id="pub-tele" type="checkbox" name="pub_tele" value="1" bind:checked={$mainSettings.values.pub_tele} class="h-4 w-4 rounded border-gray-300 text-blue-600 focus:ring-blue-500" />
                    <span>Send to telemetry topic</span>
                </label>

                <label for="pub-devices" class="flex items-center space-x-2">
                    <input id="pub-devices" type="checkbox" name="pub_devices" value="1" bind:checked={$mainSettings.values.pub_devices} class="h-4 w-4 rounded border-gray-300 text-blue-600 focus:ring-blue-500" />
                    <span>Send to devices topic</span>
                </label>
            </div>

            <!-- Updating -->
            <h2 class="text-xl font-semibold">Updating</h2>
            <div class="space-y-4">
                <label for="auto-update" class="flex items-center space-x-2">
                    <input id="auto-update" type="checkbox" name="auto_update" value="1" bind:checked={$mainSettings.values.auto_update} class="h-4 w-4 rounded border-gray-300 text-blue-600 focus:ring-blue-500" />
                    <span>Automatically update</span>
                </label>

                <label for="prerelease" class="flex items-center space-x-2">
                    <input id="prerelease" type="checkbox" name="prerelease" value="1" bind:checked={$mainSettings.values.prerelease} class="h-4 w-4 rounded border-gray-300 text-blue-600 focus:ring-blue-500" />
                    <span>Include pre-released versions in auto-update</span>
                </label>

                <label for="arduino-ota" class="flex items-center space-x-2">
                    <input id="arduino-ota" type="checkbox" name="arduino_ota" value="1" bind:checked={$mainSettings.values.arduino_ota} class="h-4 w-4 rounded border-gray-300 text-blue-600 focus:ring-blue-500" />
                    <span>Arduino OTA Update</span>
                </label>

                <div>
                    <label for="update-url" class="block text-sm font-medium"> Update URL (if set will update from this url on next boot) </label>
                    <input id="update-url" type="text" name="update" bind:value={$mainSettings.values.update} class="mt-1 block w-full rounded-md" />
                </div>
            </div>

            <div class="flex justify-end">
                <button type="submit" class="text-white bg-blue-600 hover:bg-blue-700 focus:ring-4 focus:outline-none focus:ring-blue-300 dark:bg-blue-500 dark:hover:bg-blue-600 dark:focus:ring-blue-800">
                    {s ? "Saving..." : "Save"}
                </button>
            </div>
        </form>
    {/if}
</div>


