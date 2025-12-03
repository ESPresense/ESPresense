<script lang="ts">
    import { extraSettings } from '$lib/stores.js';
    import { saveSettingsWithRetry } from '$lib/utils/settings.js';

    /** Tracks whether the form is currently being saved */
    let isSaving = $state<boolean>(false);

    /**
     * Handles form submission for extra settings.
     * Saves settings to the device and triggers a restart with automatic retry.
     */
    async function handleSubmit(event: SubmitEvent): Promise<void> {
        try {
            event.preventDefault();
            isSaving = true;
            const form = event.target as HTMLFormElement;
            const formData = new FormData(form);
            await saveSettingsWithRetry('/wifi/extras', formData, extraSettings);
        } catch (error) {
            console.error('Failed to save extra settings:', error);
            // TODO: Show user-friendly error toast notification
        } finally {
            isSaving = false;
        }
    }
</script>

<div class="bg-gray-100 dark:bg-gray-800 rounded-lg shadow p-6">
    {#if $extraSettings?.values != null}
    <form action="wifi/extras" method="post" id="extras" onsubmit={handleSubmit} class="space-y-6">
        <h2>
            <a href="https://espresense.com/configuration/settings#scanning" target="_blank">Scanning</a>
        </h2>
        <p>
            <label>
                Known BLE mac addresses (no colons, space seperated):<br />
                <input name="known_macs" bind:value={$extraSettings.values['known_macs']}/>
            </label>
        </p>
        <p>
            <label>
                Known BLE identity resolving keys, should be 32 hex chars space seperated:<br />
                <input name="known_irks" bind:value={$extraSettings.values['known_irks']}/>
            </label>
        </p>
        <p>
            <label>
                Forget beacon if not seen for (in milliseconds):<br />
                <input
                    type="number"
                    step="1"
                    min="0"
                    max="3000000"
                    name="forget_ms"
                    placeholder={$extraSettings.defaults['forget_ms']}
                    bind:value={$extraSettings.values['forget_ms']}/>
            </label>
        </p>
        <h2>
            <a href="https://espresense.com/configuration/settings#querying" target="_blank">Querying</a>
        </h2>
        <p>
            <label>
                Query device ids for characteristics (eg. flora:):<br />
                <input name="query" bind:value={$extraSettings.values['query']}/>
            </label>
        </p>
        <p>
            <label>
                Requery interval in seconds:<br />
                <input
                    type="number"
                    step="1"
                    min="30"
                    max="3600"
                    name="requery_ms"
                    placeholder={$extraSettings.defaults['requery_ms']}
                    bind:value={$extraSettings.values['requery_ms']}/>
            </label>
        </p>
        <h2>
            <a href="https://espresense.com/configuration/settings#counting" target="_blank">Counting</a>
        </h2>
        <p>
            <label>
                Include id prefixes (space seperated):<br />
                <input name="count_ids" bind:value={$extraSettings.values['count_ids']}/>
            </label>
        </p>
        <p>
            <label>
                Start counting devices less than distance (in meters):<br />
                <input
                    type="number"
                    step="0.01"
                    min="0"
                    max="100"
                    name="count_enter"
                    placeholder={$extraSettings.defaults['count_enter']}
                    bind:value={$extraSettings.values['count_enter']}/>
            </label>
        </p>
        <p>
            <label>
                Stop counting devices greater than distance (in meters):<br />
                <input
                    type="number"
                    step="0.01"
                    min="0"
                    max="100"
                    name="count_exit"
                    placeholder={$extraSettings.defaults['count_exit']}
                    bind:value={$extraSettings.values['count_exit']}/>
            </label>
        </p>
        <p>
            <label>
                Include devices with age less than (in ms):<br />
                <input
                    type="number"
                    step="1"
                    min="0"
                    max="3000000"
                    name="count_ms"
                    placeholder={$extraSettings.defaults['count_ms']}
                    bind:value={$extraSettings.values['count_ms']}/>
            </label>
        </p>
        <h2>
            <a href="https://espresense.com/configuration/settings#filtering" target="_blank">Filtering</a>
        </h2>
        <p>
            <label>
                Include only sending these ids to mqtt (eg. apple:iphone10-6 apple:iphone13-2):<br />
                <input name="include" bind:value={$extraSettings.values['include']}/>
            </label>
        </p>
        <p>
            <label>
                Exclude sending these ids to mqtt (eg. exp:20 apple:iphone10-6):<br />
                <input name="exclude" bind:value={$extraSettings.values['exclude']}/>
            </label>
        </p>
        <p>
            <label>
                Maximum distance to report (in meters):<br />
                <input
                    type="number"
                    step="0.01"
                    min="0"
                    max="100"
                    name="max_dist"
                    placeholder={$extraSettings.defaults['max_dist']}
                    bind:value={$extraSettings.values['max_dist']}/>
            </label>
        </p>
        <p>
            <label>
                Report early if beacon has moved more than this distance (in meters):<br />
                <input
                    type="number"
                    step="0.01"
                    min="0"
                    max="10"
                    name="skip_dist"
                    placeholder={$extraSettings.defaults['skip_dist']}
                    bind:value={$extraSettings.values['skip_dist']}/>
            </label>
        </p>
        <p>
            <label>
                Skip reporting if message age is less that this (in milliseconds):<br />
                <input
                    type="number"
                    step="1"
                    min="0"
                    max="3000000"
                    name="skip_ms"
                    placeholder={$extraSettings.defaults['skip_ms']}
                    bind:value={$extraSettings.values['skip_ms']}/>
            </label>
        </p>
        <h2>
            <a href="https://espresense.com/configuration/settings#calibration" target="_blank">Calibration</a>
        </h2>
        <p>
            <label>
                RSSI expected from a 0dBm transmitter at 1 meter (NOT used for iBeacons or Eddystone):<br />
                <input
                    type="number"
                    step="1"
                    min="-100"
                    max="100"
                    name="ref_rssi"
                    placeholder={$extraSettings.defaults['ref_rssi']}
                    bind:value={$extraSettings.values['ref_rssi']}/>
            </label>
        </p>
        <p>
            <label>
                RSSI adjustment for receiver (use only if you know this device has a weak antenna):<br />
                <input
                    type="number"
                    step="1"
                    min="-100"
                    max="100"
                    name="rx_adj_rssi"
                    placeholder={$extraSettings.defaults['rx_adj_rssi']}
                    bind:value={$extraSettings.values['rx_adj_rssi']}/>
            </label>
        </p>
        <p>
            <label>
                Factor used to account for absorption, reflection, or diffraction:<br />
                <input
                    type="number"
                    step="0.01"
                    min="-100"
                    max="100"
                    name="absorption"
                    placeholder={$extraSettings.defaults['absorption']}
                    bind:value={$extraSettings.values['absorption']}/>
            </label>
        </p>
        <p>
            <label>
                RSSI expected from this tx power at 1m (used for node iBeacon):<br />
                <input
                    type="number"
                    step="1"
                    min="-100"
                    max="100"
                    name="tx_ref_rssi"
                    placeholder={$extraSettings.defaults['tx_ref_rssi']}
                    bind:value={$extraSettings.values['tx_ref_rssi']}/>
            </label>
        </p>
        <div class="flex justify-end">
            <button type="submit" class="text-white bg-blue-600 hover:bg-blue-700 focus:ring-4 focus:outline-none focus:ring-blue-300 dark:bg-blue-500 dark:hover:bg-blue-600 dark:focus:ring-blue-800">
                {isSaving ? "Saving..." : "Save"}
            </button>
        </div>
    </form>
    {/if}
</div>

