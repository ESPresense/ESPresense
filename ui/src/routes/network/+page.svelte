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
                <label class="block text-sm font-medium">Room Name</label>
                <input type="text" name="room" bind:value={$mainSettings.values.room} placeholder={$mainSettings.defaults.room} class="mt-1 block w-full rounded-md" />
            </div>

            <!-- WiFi Configuration -->
            <h2 class="text-xl font-semibold">WiFi Configuration</h2>

            <div class="mb-2">
                <label class="block text-sm font-medium mb-1">WiFi SSID</label>
                <input
                    type="text"
                    name="wifi-ssid"
                    bind:value={$mainSettings.values["wifi-ssid"]}
                    placeholder="Enter WiFi SSID"
                    class="block w-full rounded-xl border-gray-600 bg-gray-800 dark:text-white px-4 py-3 text-base focus:border-blue-500 focus:ring-blue-500"
                />
            </div>

            <div>
                <div class="bg-gray-50 dark:bg-gray-900 rounded-xl shadow-sm border border-gray-200 dark:border-gray-600">
                    <div class="px-4 py-3 flex items-center justify-between border-b border-gray-200 dark:border-gray-700">
                        <label class="text-base font-medium dark:text-white">Available Networks</label>
                        {#if isScanning}
                            <div class="ios-spinner"></div>
                        {/if}
                    </div>
                    <ul class="divide-y divide-gray-200 dark:divide-gray-700">
                        {#each Object.entries(wifiNetworks) as [ssid, rssi]}
                            <li
                                onclick={() => {$mainSettings.values["wifi-ssid"] = ssid}}
                                class="flex items-center cursor-pointer px-4 py-3 hover:bg-gray-100 dark:hover:bg-gray-800 transition-colors"
                            >
                                <div class="flex-grow">
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
                                            />
                                        {/each}
                                    </div>
                                </div>
                            </li>
                        {/each}
                    </ul>
                </div>
            </div>

            <div>
                <label class="block text-sm font-medium">WiFi Password</label>
                <input type="password" name="wifi-password" bind:value={$mainSettings.values["wifi-password"]} placeholder="Enter WiFi Password" class="mt-1 block w-full rounded-md" />
            </div>

            <div>
                <label class="block text-sm font-medium"> Seconds to wait for WiFi before captive portal (-1 = forever) </label>
                <input type="number" name="wifi_timeout" bind:value={$mainSettings.values.wifi_timeout} placeholder={String($mainSettings.defaults.wifi_timeout)} step="1" min="-2147483648" max="2147483647" class="mt-1 block w-full rounded-md" />
            </div>

            <div>
                <label class="block text-sm font-medium"> Seconds to wait in captive portal before rebooting </label>
                <input type="number" name="portal_timeout" bind:value={$mainSettings.values.portal_timeout} placeholder={String($mainSettings.defaults.portal_timeout)} step="1" min="-2147483648" max="2147483647" class="mt-1 block w-full rounded-md" />
            </div>

            <div>
                <label class="block text-sm font-medium">Ethernet Type</label>
                <select name="eth" bind:value={$mainSettings.values.eth} class="mt-1 block w-full rounded-md">
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
                </select>
            </div>

            <!-- MQTT Configuration -->
            <h2 class="text-xl font-semibold">MQTT Configuration</h2>
            <div>
                <label class="block text-sm font-medium">Server</label>
                <input type="text" name="mqtt_host" bind:value={$mainSettings.values.mqtt_host} placeholder={$mainSettings.defaults.mqtt_host} class="mt-1 block w-full rounded-md" />
            </div>

            <div>
                <label class="block text-sm font-medium">Port</label>
                <input type="number" name="mqtt_port" bind:value={$mainSettings.values.mqtt_port} placeholder={String($mainSettings.defaults.mqtt_port)} step="1" min="-2147483648" max="2147483647" class="mt-1 block w-full rounded-md" />
            </div>

            <div>
                <label class="block text-sm font-medium">Username</label>
                <!-- Using a password input as in the legacy page -->
                <input type="password" name="mqtt_user" bind:value={$mainSettings.values.mqtt_user} class="mt-1 block w-full rounded-md" />
            </div>

            <div>
                <label class="block text-sm font-medium">Password</label>
                <input type="password" name="mqtt_pass" bind:value={$mainSettings.values.mqtt_pass} class="mt-1 block w-full rounded-md" />
            </div>

            <div class="space-y-4">
                <label class="flex items-center space-x-2">
                    <input type="checkbox" name="discovery" value="1" bind:checked={$mainSettings.values.discovery} class="h-4 w-4 rounded border-gray-300 text-blue-600 focus:ring-blue-500" />
                    <span>Send to discovery topic</span>
                </label>

                <div class="ml-6">
                    <label class="block text-sm font-medium"> Home Assistant discovery topic prefix </label>
                    <input type="text" name="discovery_prefix" bind:value={$mainSettings.values.discovery_prefix} placeholder={$mainSettings.defaults.discovery_prefix} class="mt-1 block w-full rounded-md" />
                </div>

                <label class="flex items-center space-x-2">
                    <input type="checkbox" name="pub_tele" value="1" bind:checked={$mainSettings.values.pub_tele} class="h-4 w-4 rounded border-gray-300 text-blue-600 focus:ring-blue-500" />
                    <span>Send to telemetry topic</span>
                </label>

                <label class="flex items-center space-x-2">
                    <input type="checkbox" name="pub_devices" value="1" bind:checked={$mainSettings.values.pub_devices} class="h-4 w-4 rounded border-gray-300 text-blue-600 focus:ring-blue-500" />
                    <span>Send to devices topic</span>
                </label>
            </div>

            <!-- Updating -->
            <h2 class="text-xl font-semibold">Updating</h2>
            <div class="space-y-4">
                <label class="flex items-center space-x-2">
                    <input type="checkbox" name="auto_update" value="1" bind:checked={$mainSettings.values.auto_update} class="h-4 w-4 rounded border-gray-300 text-blue-600 focus:ring-blue-500" />
                    <span>Automatically update</span>
                </label>

                <label class="flex items-center space-x-2">
                    <input type="checkbox" name="prerelease" value="1" bind:checked={$mainSettings.values.prerelease} class="h-4 w-4 rounded border-gray-300 text-blue-600 focus:ring-blue-500" />
                    <span>Include pre-released versions in auto-update</span>
                </label>

                <label class="flex items-center space-x-2">
                    <input type="checkbox" name="arduino_ota" value="1" bind:checked={$mainSettings.values.arduino_ota} class="h-4 w-4 rounded border-gray-300 text-blue-600 focus:ring-blue-500" />
                    <span>Arduino OTA Update</span>
                </label>

                <div>
                    <label class="block text-sm font-medium"> Update URL (if set will update from this url on next boot) </label>
                    <input type="text" name="update" bind:value={$mainSettings.values.update} class="mt-1 block w-full rounded-md" />
                </div>
            </div>

            <div class="flex justify-end">
                <button type="submit" class="px-4 py-2 text-sm font-medium text-white bg-blue-600 hover:bg-blue-700 focus:ring-4 focus:outline-none focus:ring-blue-300 rounded-lg dark:bg-blue-500 dark:hover:bg-blue-600 dark:focus:ring-blue-800">
                    {s ? "Saving..." : "Save"}
                </button>
            </div>
        </form>
    {/if}
</div>

<style>
    :global(input:not([type="checkbox"])) {
        @apply border-2 border-gray-300 bg-white focus:border-blue-500 focus:ring-blue-500;
    }
    :global(.dark input:not([type="checkbox"])) {
        @apply border-gray-500 bg-gray-900 text-white;
    }

    :global(select) {
        @apply border-2 border-gray-300 bg-white text-gray-900 focus:border-blue-500 focus:ring-blue-500;
    }
    :global(.dark select) {
        @apply border-gray-500 bg-gray-900 text-white;
    }

    :global(input[type="checkbox"]) {
        @apply h-4 w-4 rounded border-gray-300 text-blue-600 focus:ring-blue-500;
    }
    :global(.dark input[type="checkbox"]) {
        @apply border-gray-600;
    }

    :global(h2) {
        @apply text-gray-900 mb-4;
    }
    :global(.dark h2) {
        @apply text-white;
    }

    :global(label) {
        @apply text-gray-700 mb-1;
    }
    :global(.dark label) {
        @apply text-white;
    }

    .ios-spinner {
        width: 16px;
        height: 16px;
        border: 2px solid transparent;
        border-top-color: #3b82f6;
        border-right-color: #3b82f6;
        border-bottom-color: #3b82f6;
        border-radius: 50%;
        animation: spin 1s cubic-bezier(0.42, 0.61, 0.58, 0.41) infinite;
        opacity: 0.8;
    }

    @keyframes spin {
        0% { transform: rotate(0deg); }
        100% { transform: rotate(360deg); }
    }

    :global(.dark .ios-spinner) {
        border-top-color: #60a5fa;
        border-right-color: #60a5fa;
        border-bottom-color: #60a5fa;
    }
</style>
