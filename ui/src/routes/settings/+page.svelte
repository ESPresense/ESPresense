<script lang="ts">
    import { extraSettings } from '$lib/stores.js';

    let s = $state(false);
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
            await fetch("/wifi/extras", { method: "POST", body: params });

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
                    const response = await fetch("/wifi/extras");
                    data = await response.json();
                    extraSettings.set(data);
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
        <h2>
            <a href="https://espresense.com/configuration/settings#leds" target="_blank">LEDs</a>
        </h2>
        <h4>LED 1:</h4>
        <p>
            <label>
                LED Type:<br />
                <select name="led_1_type" bind:value={$extraSettings.values['led_1_type']}>
                    <option disabled selected hidden>PWM</option>
                    <option value="0">PWM</option>
                    <option value="1">PWM Inverted</option>
                    <option value="2">Addressable GRB</option>
                    <option value="3">Addressable GRBW</option>
                    <option value="4">Addressable RGB</option>
                    <option value="5">Addressable RGBW</option>
                </select>
            </label>
        </p>
        <p>
            <label>
                Pin (-1 to disable):<br />
                <input
                    type="number"
                    step="1"
                    min="-1"
                    max="39"
                    name="led_1_pin"
                    placeholder={$extraSettings.defaults['led_1_pin']}
                    bind:value={$extraSettings.values['led_1_pin']}/>
            </label>
        </p>
        <p>
            <label>
                Count (only applies to Addressable LEDs):<br />
                <input
                    type="number"
                    step="1"
                    min="-1"
                    max="39"
                    name="led_1_cnt"
                    placeholder={$extraSettings.defaults['led_1_cnt']}
                    bind:value={$extraSettings.values['led_1_cnt']}/>
            </label>
        </p>
        <p>
            <label>
                LED Control:<br />
                <select name="led_1_cntrl" bind:value={$extraSettings.values['led_1_cntrl']}>
                    <option disabled selected hidden>Status</option>
                    <option value="0">MQTT</option>
                    <option value="1">Status</option>
                    <option value="2">Motion</option>
                    <option value="3">Count</option>
                </select>
            </label>
        </p>
        <h4>LED 2:</h4>
        <p>
            <label>
                LED Type:<br />
                <select name="led_2_type" bind:value={$extraSettings.values['led_2_type']}>
                    <option disabled selected hidden>PWM</option>
                    <option value="0">PWM</option>
                    <option value="1">PWM Inverted</option>
                    <option value="2">Addressable GRB</option>
                    <option value="3">Addressable GRBW</option>
                    <option value="4">Addressable RGB</option>
                    <option value="5">Addressable RGBW</option>
                </select>
            </label>
        </p>
        <p>
            <label>
                Pin (-1 to disable):<br />
                <input
                    type="number"
                    step="1"
                    min="-1"
                    max="39"
                    name="led_2_pin"
                    placeholder={$extraSettings.defaults['led_2_pin']}
                    bind:value={$extraSettings.values['led_2_pin']}/>
            </label>
        </p>
        <p>
            <label>
                Count (only applies to Addressable LEDs):<br />
                <input
                    type="number"
                    step="1"
                    min="-1"
                    max="39"
                    name="led_2_cnt"
                    placeholder={$extraSettings.defaults['led_2_cnt']}
                    bind:value={$extraSettings.values['led_2_cnt']}/>
            </label>
        </p>
        <p>
            <label>
                LED Control:<br />
                <select name="led_2_cntrl" bind:value={$extraSettings.values['led_2_cntrl']}>
                    <option disabled selected hidden>MQTT</option>
                    <option value="0">MQTT</option>
                    <option value="1">Status</option>
                    <option value="2">Motion</option>
                    <option value="3">Count</option>
                </select>
            </label>
        </p>
        <h4>LED 3:</h4>
        <p>
            <label>
                LED Type:<br />
                <select name="led_3_type" bind:value={$extraSettings.values['led_3_type']}>
                    <option disabled selected hidden>PWM</option>
                    <option value="0">PWM</option>
                    <option value="1">PWM Inverted</option>
                    <option value="2">Addressable GRB</option>
                    <option value="3">Addressable GRBW</option>
                    <option value="4">Addressable RGB</option>
                    <option value="5">Addressable RGBW</option>
                </select>
            </label>
        </p>
        <p>
            <label>
                Pin (-1 to disable):<br />
                <input
                    type="number"
                    step="1"
                    min="-1"
                    max="39"
                    name="led_3_pin"
                    placeholder={$extraSettings.defaults['led_3_pin']}
                    bind:value={$extraSettings.values['led_3_pin']}/>
            </label>
        </p>
        <p>
            <label>
                Count (only applies to Addressable LEDs):<br />
                <input
                    type="number"
                    step="1"
                    min="-1"
                    max="39"
                    name="led_3_cnt"
                    placeholder={$extraSettings.defaults['led_3_cnt']}
                    bind:value={$extraSettings.values['led_3_cnt']}/>
            </label>
        </p>
        <p>
            <label>
                LED Control:<br />
                <select name="led_3_cntrl" bind:value={$extraSettings.values['led_3_cntrl']}>
                    <option disabled selected hidden>MQTT</option>
                    <option value="0">MQTT</option>
                    <option value="1">Status</option>
                    <option value="2">Motion</option>
                    <option value="3">Count</option>
                </select>
            </label>
        </p>
        <h2>
            <a href="https://espresense.com/configuration/settings#gpio-sensors" target="_blank">GPIO Sensors</a>
        </h2>
        <h4>PIR:</h4>
        <p>
            <label>
                PIR motion pin type:<br />
                <select name="pir_type" bind:value={$extraSettings.values['pir_type']}>
                    <option disabled selected hidden>Pullup</option>
                    <option value="0">Pullup</option>
                    <option value="1">Pullup Inverted</option>
                    <option value="2">Pulldown</option>
                    <option value="3">Pulldown Inverted</option>
                    <option value="4">Floating</option>
                    <option value="5">Floating Inverted</option>
                </select>
            </label>
        </p>
        <p>
            <label>
                PIR motion pin (-1 for disable):<br />
                <input
                    type="number"
                    step="1"
                    name="pir_pin"
                    placeholder={$extraSettings.defaults['pir_pin']}
                    bind:value={$extraSettings.values['pir_pin']}/>
            </label>
        </p>
        <p>
            <label>
                PIR motion timeout (in seconds):<br />
                <input
                    type="number"
                    step="0.01"
                    min="0"
                    max="300"
                    name="pir_timeout"
                    placeholder={$extraSettings.defaults['pir_timeout']}
                    bind:value={$extraSettings.values['pir_timeout']}/>
            </label>
        </p>
        <h4>Radar:</h4>
        <p>
            <label>
                Radar motion pin type:<br />
                <select name="radar_type" bind:value={$extraSettings.values['radar_type']}>
                    <option disabled selected hidden>Pullup</option>
                    <option value="0">Pullup</option>
                    <option value="1">Pullup Inverted</option>
                    <option value="2">Pulldown</option>
                    <option value="3">Pulldown Inverted</option>
                    <option value="4">Floating</option>
                    <option value="5">Floating Inverted</option>
                </select>
            </label>
        </p>
        <p>
            <label>
                Radar motion pin (-1 for disable):<br />
                <input
                    type="number"
                    step="1"
                    name="radar_pin"
                    placeholder={$extraSettings.defaults['radar_pin']}
                    bind:value={$extraSettings.values['radar_pin']}/>
            </label>
        </p>
        <p>
            <label>
                Radar motion timeout (in seconds):<br />
                <input
                    type="number"
                    step="0.01"
                    min="0"
                    max="300"
                    name="radar_timeout"
                    placeholder={$extraSettings.defaults['radar_timeout']}
                    bind:value={$extraSettings.values['radar_timeout']}/>
            </label>
        </p>
        <h4>Switch One:</h4>
        <p>
            <label>
                Switch One pin type:<br />
                <select name="switch_1_type" bind:value={$extraSettings.values['switch_1_type']}>
                    <option disabled selected hidden>Pullup</option>
                    <option value="0">Pullup</option>
                    <option value="1">Pullup Inverted</option>
                    <option value="2">Pulldown</option>
                    <option value="3">Pulldown Inverted</option>
                    <option value="4">Floating</option>
                    <option value="5">Floating Inverted</option>
                </select>
            </label>
        </p>
        <p>
            <label>
                Switch One pin (-1 for disable):<br />
                <input
                    type="number"
                    step="1"
                    name="switch_1_pin"
                    placeholder={$extraSettings.defaults['switch_1_pin']}
                    bind:value={$extraSettings.values['switch_1_pin']}/>
            </label>
        </p>
        <p>
            <label>
                Switch One timeout (in seconds):<br />
                <input
                    type="number"
                    step="0.01"
                    min="0"
                    max="300"
                    name="switch_1_timeout"
                    placeholder={$extraSettings.defaults['switch_1_timeout']}
                    bind:value={$extraSettings.values['switch_1_timeout']}/>
            </label>
        </p>
        <h4>Switch Two:</h4>
        <p>
            <label>
                Switch Two pin type:<br />
                <select name="switch_2_type" bind:value={$extraSettings.values['switch_2_type']}>
                    <option disabled selected hidden>Pullup</option>
                    <option value="0">Pullup</option>
                    <option value="1">Pullup Inverted</option>
                    <option value="2">Pulldown</option>
                    <option value="3">Pulldown Inverted</option>
                    <option value="4">Floating</option>
                    <option value="5">Floating Inverted</option>
                </select>
            </label>
        </p>
        <p>
            <label>
                Switch Two pin (-1 for disable):<br />
                <input
                    type="number"
                    step="1"
                    name="switch_2_pin"
                    placeholder={$extraSettings.defaults['switch_2_pin']}
                    bind:value={$extraSettings.values['switch_2_pin']}/>
            </label>
        </p>
        <p>
            <label>
                Switch Two timeout (in seconds):<br />
                <input
                    type="number"
                    step="0.01"
                    min="0"
                    max="300"
                    name="switch_2_timeout"
                    placeholder={$extraSettings.defaults['switch_2_timeout']}
                    bind:value={$extraSettings.values['switch_2_timeout']}/>
            </label>
        </p>
        <h4>Button One:</h4>
        <p>
            <label>
                Button One pin type:<br />
                <select name="button_1_type" bind:value={$extraSettings.values['button_1_type']}>
                    <option disabled selected hidden>Pullup</option>
                    <option value="0">Pullup</option>
                    <option value="1">Pullup Inverted</option>
                    <option value="2">Pulldown</option>
                    <option value="3">Pulldown Inverted</option>
                    <option value="4">Floating</option>
                    <option value="5">Floating Inverted</option>
                </select>
            </label>
        </p>
        <p>
            <label>
                Button One pin (-1 for disable):<br />
                <input
                    type="number"
                    step="1"
                    name="button_1_pin"
                    placeholder={$extraSettings.defaults['button_1_pin']}
                    bind:value={$extraSettings.values['button_1_pin']}/>
            </label>
        </p>
        <p>
            <label>
                Button One timeout (in seconds):<br />
                <input
                    type="number"
                    step="0.01"
                    min="0"
                    max="300"
                    name="button_1_timeout"
                    placeholder={$extraSettings.defaults['button_1_timeout']}
                    bind:value={$extraSettings.values['button_1_timeout']}/>
            </label>
        </p>
        <h4>Button Two:</h4>
        <p>
            <label>
                Button Two pin (-1 for disable):<br />
                <input
                    type="number"
                    step="1"
                    name="button_2_pin"
                    placeholder={$extraSettings.defaults['button_2_pin']}
                    bind:value={$extraSettings.values['button_2_pin']}/>
            </label>
        </p>
        <p>
            <label>
                Button Two timeout (in seconds):<br />
                <input
                    type="number"
                    step="0.01"
                    min="0"
                    max="300"
                    name="button_2_timeout"
                    placeholder={$extraSettings.defaults['button_2_timeout']}
                    bind:value={$extraSettings.values['button_2_timeout']}/>
            </label>
        </p>
        <h4>DHT:</h4>
        <p>
            <label>
                DHT11 sensor pin (-1 for disable):<br />
                <input
                    type="number"
                    step="1"
                    name="dht11_pin"
                    placeholder={$extraSettings.defaults['dht11_pin']}
                    bind:value={$extraSettings.values['dht11_pin']}/>
            </label>
        </p>
        <p>
            <label>
                DHT22 sensor pin (-1 for disable):<br />
                <input
                    type="number"
                    step="1"
                    name="dht22_pin"
                    placeholder={$extraSettings.defaults['dht22_pin']}
                    bind:value={$extraSettings.values['dht22_pin']}/>
            </label>
        </p>
        <p>
            <label>
                DHT temperature offset:<br />
                <input
                    type="number"
                    step="0.01"
                    min="-40"
                    max="125"
                    name="dhtTemp_offset"
                    placeholder={$extraSettings.defaults['dhtTemp_offset']}
                    bind:value={$extraSettings.values['dhtTemp_offset']}/>
            </label>
        </p>
        <h2>
            <a href="https://espresense.com/configuration/settings#i2c-settings" target="_blank">I2C Settings</a>
        </h2>
        <h4>Bus 1:</h4>
        <p>
            <label>
                SDA pin (-1 to disable):<br />
                <input
                    type="number"
                    step="1"
                    min="-1"
                    max="39"
                    name="I2C_Bus_1_SDA"
                    placeholder={$extraSettings.defaults['I2C_Bus_1_SDA']}
                    bind:value={$extraSettings.values['I2C_Bus_1_SDA']}/>
            </label>
        </p>
        <p>
            <label>
                SCL pin (-1 to disable):<br />
                <input
                    type="number"
                    step="1"
                    min="-1"
                    max="39"
                    name="I2C_Bus_1_SCL"
                    placeholder={$extraSettings.defaults['I2C_Bus_1_SCL']}
                    bind:value={$extraSettings.values['I2C_Bus_1_SCL']}/>
            </label>
        </p>
        <h4>Bus 2:</h4>
        <p>
            <label>
                SDA pin (-1 to disable):<br />
                <input
                    type="number"
                    step="1"
                    min="-1"
                    max="39"
                    name="I2C_Bus_2_SDA"
                    placeholder={$extraSettings.defaults['I2C_Bus_2_SDA']}
                    bind:value={$extraSettings.values['I2C_Bus_2_SDA']}/>
            </label>
        </p>
        <p>
            <label>
                SCL pin (-1 to disable):<br />
                <input
                    type="number"
                    step="1"
                    min="-1"
                    max="39"
                    name="I2C_Bus_2_SCL"
                    placeholder={$extraSettings.defaults['I2C_Bus_2_SCL']}
                    bind:value={$extraSettings.values['I2C_Bus_2_SCL']}/>
            </label>
        </p>
        <p>
            <label class="flex items-center space-x-2">
                <input type="checkbox" name="I2CDebug" bind:checked={$extraSettings.values['I2CDebug']}/>
                <span>Debug I2C addreses. Look at the serial log to get the correct address (default: &#x2610;)</span>
            </label>
        </p>
        <h2>
            <a href="https://espresense.com/configuration/settings#i2c-sensors" target="_blank">I2C Sensors</a>
        </h2>
        <h4>AHTX0 - Temperature + Humidity Sensor:</h4>
        <p>
            <label>
                I2C Bus:<br />
                <input
                    type="number"
                    step="1"
                    min="1"
                    max="2"
                    name="AHTX0_I2c_Bus"
                    placeholder={$extraSettings.defaults['AHTX0_I2c_Bus']}
                    bind:value={$extraSettings.values['AHTX0_I2c_Bus']}/>
            </label>
        </p>
        <p>
            <label>
                I2C address (0x38 or 0x39):<br />
                <input name="AHTX0_I2c" bind:value={$extraSettings.values['AHTX0_I2c']}/>
            </label>
        </p>
        <h4>BH1750 - Ambient Light Sensor:</h4>
        <p>
            <label>
                I2C Bus:<br />
                <input
                    type="number"
                    step="1"
                    min="1"
                    max="2"
                    name="BH1750_I2c_Bus"
                    placeholder={$extraSettings.defaults['BH1750_I2c_Bus']}
                    bind:value={$extraSettings.values['BH1750_I2c_Bus']}/>
            </label>
        </p>
        <p>
            <label>
                I2C address (0x23 or 0x5C):<br />
                <input name="BH1750_I2c" bind:value={$extraSettings.values['BH1750_I2c']}/>
            </label>
        </p>
        <h4>BME280 - Humidity + Temp + Pressure Sensor:</h4>
        <p>
            <label>
                I2C Bus:<br />
                <input
                    type="number"
                    step="1"
                    min="1"
                    max="2"
                    name="BME280_I2c_Bus"
                    placeholder={$extraSettings.defaults['BME280_I2c_Bus']}
                    bind:value={$extraSettings.values['BME280_I2c_Bus']}/>
            </label>
        </p>
        <p>
            <label>
                I2C address (0x76 or 0x77):<br />
                <input name="BME280_I2c" bind:value={$extraSettings.values['BME280_I2c']}/>
            </label>
        </p>
        <h4>BMP085/BMP180 - Barometric Pressure + Temperature:</h4>
        <p>
            <label>
                I2C Bus:<br />
                <input
                    type="number"
                    step="1"
                    min="1"
                    max="2"
                    name="BMP180_I2c_Bus"
                    placeholder={$extraSettings.defaults['BMP180_I2c_Bus']}
                    bind:value={$extraSettings.values['BMP180_I2c_Bus']}/>
            </label>
        </p>
        <p>
            <label>
                I2C address (0x77):<br />
                <input name="BMP180_I2c" bind:value={$extraSettings.values['BMP180_I2c']}/>
            </label>
        </p>
        <h4>BMP280 - Barometric Pressure + Temperature Sensor:</h4>
        <p>
            <label>
                I2C Bus:<br />
                <input
                    type="number"
                    step="1"
                    min="1"
                    max="2"
                    name="BMP280_I2c_Bus"
                    placeholder={$extraSettings.defaults['BMP280_I2c_Bus']}
                    bind:value={$extraSettings.values['BMP280_I2c_Bus']}/>
            </label>
        </p>
        <p>
            <label>
                I2C address (0x76 or 0x77):<br />
                <input name="BMP280_I2c" bind:value={$extraSettings.values['BMP280_I2c']}/>
            </label>
        </p>
        <h4>SHTC1/3, SHTW1/2, SHT3x/4x, SHT85 - Temperature and Humidity Sensor:</h4>
        <p>
            <label>
                I2C Bus (-1 to disable):<br />
                <input
                    type="number"
                    step="1"
                    min="1"
                    max="2"
                    name="SHT_I2c_Bus"
                    placeholder={$extraSettings.defaults['SHT_I2c_Bus']}
                    bind:value={$extraSettings.values['SHT_I2c_Bus']}/>
            </label>
        </p>
        <h4>TSL2561 - Ambient Light Sensor:</h4>
        <p>
            <label>
                I2C Bus:<br />
                <input
                    type="number"
                    step="1"
                    min="1"
                    max="2"
                    name="TSL2561_I2c_Bus"
                    placeholder={$extraSettings.defaults['TSL2561_I2c_Bus']}
                    bind:value={$extraSettings.values['TSL2561_I2c_Bus']}/>
            </label>
        </p>
        <p>
            <label>
                I2C address (0x39, 0x49 or 0x29):<br />
                <input name="TSL2561_I2c" bind:value={$extraSettings.values['TSL2561_I2c']}/>
            </label>
        </p>
        <p>
            <label>
                Gain (auto, 1x or 16x):<br />
                <input
                    name="TSL2561_I2c_Gain"
                    placeholder={$extraSettings.defaults['TSL2561_I2c_Gain']}
                    bind:value={$extraSettings.values['TSL2561_I2c_Gain']}/>
            </label>
        </p>
        <h4>SGP30 - Air Quality Sensor:</h4>
        <p>
            <label>
                I2C Bus:<br />
                <input
                    type="number"
                    step="1"
                    min="1"
                    max="2"
                    name="SGP30_I2c_Bus"
                    placeholder={$extraSettings.defaults['SGP30_I2c_Bus']}
                    bind:value={$extraSettings.values['SGP30_I2c_Bus']}/>
            </label>
        </p>
        <p>
            <label>
                I2C address (0x58):<br />
                <input name="SGP30_I2c" bind:value={$extraSettings.values['SGP30_I2c']}/>
            </label>
        </p>
        <h4>SCD4x - CO₂, Temperature and Humidity Sensor:</h4>
        <p>
            <label>
                I2C Bus:<br />
                <input
                    type="number"
                    step="1"
                    min="1"
                    max="2"
                    name="SCD4x_I2c_Bus"
                    placeholder={$extraSettings.defaults['SCD4x_I2c_Bus']}
                    bind:value={$extraSettings.values['SCD4x_I2c_Bus']}/>
            </label>
        </p>
        <p>
            <label>
                I2C address (0x62):<br />
                <input name="SCD4x_I2c" bind:value={$extraSettings.values['SCD4x_I2c']}/>
            </label>
        </p>
        <h4>HX711 - Weight Sensor:</h4>
        <p>
            <label>
                HX711 SCK (Clock) pin:<br />
                <input
                    type="number"
                    step="1"
                    name="HX711_sckPin"
                    placeholder={$extraSettings.defaults['HX711_sckPin']}
                    bind:value={$extraSettings.values['HX711_sckPin']}/>
            </label>
        </p>
        <p>
            <label>
                HX711 DOUT (Data) pin:<br />
                <input
                    type="number"
                    step="1"
                    name="HX711_doutPin"
                    placeholder={$extraSettings.defaults['HX711_doutPin']}
                    bind:value={$extraSettings.values['HX711_doutPin']}/>
            </label>
        </p>
        <h4>DS18B20:</h4>
        <p>
            <label>
                DS18B20 sensor pin (-1 for disable):<br />
                <input
                    type="number"
                    step="1"
                    name="ds18b20_pin"
                    placeholder={$extraSettings.defaults['ds18b20_pin']}
                    bind:value={$extraSettings.values['ds18b20_pin']}/>
            </label>
        </p>
        <p>
            <label>
                DS18B20 temperature offset:<br />
                <input
                    type="number"
                    step="0.01"
                    min="-40"
                    max="125"
                    name="dsTemp_offset"
                    placeholder={$extraSettings.defaults['dsTemp_offset']}
                    bind:value={$extraSettings.values['dsTemp_offset']}/>
            </label>
        </p>
        <div class="flex justify-end">
            <button type="submit" class="text-white bg-blue-600 hover:bg-blue-700 focus:ring-4 focus:outline-none focus:ring-blue-300 dark:bg-blue-500 dark:hover:bg-blue-600 dark:focus:ring-blue-800">
                {s ? "Saving..." : "Save"}
            </button>
        </div>
    </form>
    {/if}
</div>


