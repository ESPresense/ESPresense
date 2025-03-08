<script lang="ts">
    import { extraSettings } from '$lib/stores.js';
    import LocaleSelector from "$lib/components/LocaleSelector.svelte";
    import { _, setupI18n } from '../../lang/i18n'

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
    let lang = $state(localStorage.getItem('locale') || 'en')

</script>

<div class="bg-gray-100 dark:bg-gray-800 rounded-lg shadow p-6">
    {#if $extraSettings?.values != null}
    <form action="wifi/extras" method="post" id="extras" onsubmit={handleSubmit} class="space-y-6">
        <h2>
            {$_('settings.locale_language')}
        </h2>
        <LocaleSelector
            bind:value={lang}
            on:locale-changed={e =>
            setupI18n({ withLocale: e.detail }) }
        />

        <h2>
            <a href="https://espresense.com/configuration/settings#scanning" target="_blank">{$_('settings.scanning')}</a>
        </h2>
        <p>
            <label>
                {$_('settings.known_ble_mac_addresses')}<br />
                <input name="known_macs" bind:value={$extraSettings.values['known_macs']}/>
            </label>
        </p>
        <p>
            <label>
                {$_('settings.known_ble_identity_keys')}<br />
                <input name="known_irks" bind:value={$extraSettings.values['known_irks']}/>
            </label>
        </p>
        <p>
            <label>
                {$_('settings.forget_beacon_if_not_seen')}<br />
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
            <a href="https://espresense.com/configuration/settings#querying" target="_blank">{$_('settings.querying')}</a>
        </h2>
        <p>
            <label>
                {$_('settings.query_device_ids_for_char')}<br />
                <input name="query" bind:value={$extraSettings.values['query']}/>
            </label>
        </p>
        <p>
            <label>
                {$_('settings.requery_interval')}<br />
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
            <a href="https://espresense.com/configuration/settings#counting" target="_blank">{$_('settings.counting')}</a>
        </h2>
        <p>
            <label>
                {$_('settings.include_id_prefixes')}<br />
                <input name="count_ids" bind:value={$extraSettings.values['count_ids']}/>
            </label>
        </p>
        <p>
            <label>
                {$_('settings.start_counting_devices')}<br />
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
                {$_('settings.stop_counting_devices')}<br />
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
                {$_('settings.include_devices_with_age')}<br />
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
            <a href="https://espresense.com/configuration/settings#filtering" target="_blank">{$_('settings.filtering')}</a>
        </h2>
        <p>
            <label>
                {$_('settings.include_only_sending_ids')}<br />
                <input name="include" bind:value={$extraSettings.values['include']}/>
            </label>
        </p>
        <p>
            <label>
                {$_('settings.exclude_sending_ids')}<br />
                <input name="exclude" bind:value={$extraSettings.values['exclude']}/>
            </label>
        </p>
        <p>
            <label>
                {$_('settings.maximum_distance_to_report')}<br />
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
                {$_('settings.report_early_if_beacon_moved')}<br />
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
                {$_('settings.skip_reporting_if_message_age')}<br />
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
            <a href="https://espresense.com/configuration/settings#calibration" target="_blank">{$_('settings.calibration')}</a>
        </h2>
        <p>
            <label>
                {$_('settings.rssi_expected_from_0dbm')}<br />
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
                {$_('settings.rssi_adjustmemt_for_receiver')}<br />
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
                {$_('settings.factor_used_to_account')}<br />
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
                {$_('settings.rssi_expected_from_tx')}<br />
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
            <a href="https://espresense.com/configuration/settings#leds" target="_blank">{$_('settings.leds')}</a>
        </h2>
        <h4>LED 1:</h4>
        <p>
            <label>
                {$_('settings.led_type')}<br />
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
                {$_('settings.pin')}<br />
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
                {$_('settings.count')}<br />
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
                {$_('settings.led_control')}<br />
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
                {$_('settings.led_type')}<br />
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
                {$_('settings.pin')}<br />
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
                {$_('settings.count')}<br />
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
                {$_('settings.led_control')}<br />
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
                {$_('settings.led_type')}<br />
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
                {$_('settings.pin')}<br />
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
                {$_('settings.count')}<br />
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
                {$_('settings.led_control')}<br />
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
            <a href="https://espresense.com/configuration/settings#gpio-sensors" target="_blank">{$_('settings.gpio_sensors')}</a>
        </h2>
        <h4>{$_('settings.pir')}</h4>
        <p>
            <label>
                {$_('settings.pir_motion_pin_type')}<br />
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
                {$_('settings.pir_motion_pin')}<br />
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
                {$_('settings.pir_motion_timeout')}<br />
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
        <h4>{$_('settings.radar')}</h4>
        <p>
            <label>
                {$_('settings.radar_motion_pin_type')}<br />
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
                {$_('settings.radar_motion_pin')}<br />
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
                {$_('settings.radar_motion_timeout')}<br />
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
        <h4>{$_('settings.switch_one')}</h4>
        <p>
            <label>
                {$_('settings.switch_one_pin_type')}<br />
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
                {$_('settings.switch_one_pin')}<br />
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
                {$_('settings.switch_one_timeout')}<br />
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
        <h4>{$_('settings.switch_two')}</h4>
        <p>
            <label>
                {$_('settings.switch_two_pin_type')}<br />
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
                {$_('settings.switch_two_pin')}<br />
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
                {$_('settings.switch_two_timeout')}<br />
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
        <h4>{$_('settings.button_one')}</h4>
        <p>
            <label>
                {$_('settings.button_one_pin_type')}<br />
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
                {$_('settings.button_one_pin')}<br />
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
                {$_('settings.button_one_timeout')}<br />
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
        <h4>{$_('settings.button_two')}</h4>
        <p>
            <label>
                {$_('settings.button_two_pin_type')}<br />
                <select name="button_2_type" bind:value={$extraSettings.values['button_2_type']}>
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
                {$_('settings.button_two_pin')}<br />
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
                {$_('settings.button_two_timeout')}<br />
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
                {$_('settings.dht11_sensor_pin')}<br />
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
                {$_('settings.dht22_sensor_pin')}<br />
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
                {$_('settings.dht_temperature_offset')}<br />
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
            <a href="https://espresense.com/configuration/settings#i2c-settings" target="_blank">{$_('settings.i2c_settings')}</a>
        </h2>
        <h4>Bus 1:</h4>
        <p>
            <label>
                {$_('settings.sda_pin')}<br />
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
                {$_('settings.scl_pin')}<br />
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
                {$_('settings.sda_pin')}<br />
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
                {$_('settings.scl_pin')}<br />
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
                <span>{$_('settings.debug_i2c_addresses')}</span>
            </label>
        </p>
        <h2>
            <a href="https://espresense.com/configuration/settings#i2c-sensors" target="_blank">{$_('settings.i2c_sensors')}</a>
        </h2>
        <h4>{$_('settings.ahtx0_sensor')}</h4>
        <p>
            <label>
                {$_('settings.i2c_bus')}<br />
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
                {$_('settings.i2c_address_38_or_39')}<br />
                <input name="AHTX0_I2c" bind:value={$extraSettings.values['AHTX0_I2c']}/>
            </label>
        </p>
        <h4>{$_('settings.bh1750_sensor')}</h4>
        <p>
            <label>
                {$_('settings.i2c_bus')}<br />
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
                {$_('settings.i2c_address_23_or_5c')}<br />
                <input name="BH1750_I2c" bind:value={$extraSettings.values['BH1750_I2c']}/>
            </label>
        </p>
        <h4>{$_('settings.bme280_sensor')}</h4>
        <p>
            <label>
                {$_('settings.i2c_bus')}<br />
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
                {$_('settings.i2c_address_76_or_77')}<br />
                <input name="BME280_I2c" bind:value={$extraSettings.values['BME280_I2c']}/>
            </label>
        </p>
        <h4>{$_('settings.bmp085_bmp180_sensor')}</h4>
        <p>
            <label>
                {$_('settings.i2c_bus')}<br />
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
                {$_('settings.i2c_address_77')}<br />
                <input name="BMP180_I2c" bind:value={$extraSettings.values['BMP180_I2c']}/>
            </label>
        </p>
        <h4>{$_('settings.bmp280_sensor')}</h4>
        <p>
            <label>
                {$_('settings.i2c_bus')}<br />
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
                {$_('settings.i2c_address_76_or_77')}<br />
                <input name="BMP280_I2c" bind:value={$extraSettings.values['BMP280_I2c']}/>
            </label>
        </p>
        <h4>{$_('settings.sht_sensor')}</h4>
        <p>
            <label>
                {$_('settings.i2c_bus_setting')}<br />
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
        <h4>{$_('settings.tsl2561_sensor')}</h4>
        <p>
            <label>
                {$_('settings.i2c_bus')}<br />
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
                {$_('settings.i2c_address_39_48_or_29')}<br />
                <input name="TSL2561_I2c" bind:value={$extraSettings.values['TSL2561_I2c']}/>
            </label>
        </p>
        <p>
            <label>
                {$_('settings.gain')}<br />
                <input
                    name="TSL2561_I2c_Gain"
                    placeholder={$extraSettings.defaults['TSL2561_I2c_Gain']}
                    bind:value={$extraSettings.values['TSL2561_I2c_Gain']}/>
            </label>
        </p>
        <h4>{$_('settings.sgp30_sensor')}</h4>
        <p>
            <label>
                {$_('settings.i2c_bus')}<br />
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
                {$_('settings.i2c_address_58')}<br />
                <input name="SGP30_I2c" bind:value={$extraSettings.values['SGP30_I2c']}/>
            </label>
        </p>
        <h4>{$_('settings.hx711_sensor')}</h4>
        <p>
            <label>
                {$_('settings.hx711_sck')}<br />
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
                {$_('settings.hx711_dout')}<br />
                <input
                    type="number"
                    step="1"
                    name="HX711_doutPin"
                    placeholder={$extraSettings.defaults['HX711_doutPin']}
                    bind:value={$extraSettings.values['HX711_doutPin']}/>
            </label>
        </p>
        <h4>{$_('settings.ds18b20_sensor')}</h4>
        <p>
            <label>
                {$_('settings.ds18b20_sensor_pin')}<br />
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
                {$_('settings.ds18b20_temperature_offset')}<br />
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
            <button type="submit" class="px-4 py-2 text-sm font-medium text-white bg-blue-600 hover:bg-blue-700 focus:ring-4 focus:outline-none focus:ring-blue-300 rounded-lg dark:bg-blue-500 dark:hover:bg-blue-600 dark:focus:ring-blue-800">
                {s ? $_('settings.saving') : $_('settings.save')}
            </button>
        </div>
    </form>
    {/if}
</div>

<style>
    :global(input:not([type^="checkbox"])) {
        @apply mt-1 block w-full border-2 border-gray-300 bg-white focus:border-blue-500 focus:ring-blue-500;
    }
    :global(.dark input:not([type^="checkbox"])) {
        @apply border-gray-500 bg-gray-900 text-white;
    }

    :global(select) {
        @apply mt-1 block w-full border-2 border-gray-300 bg-white text-gray-900 focus:border-blue-500 focus:ring-blue-500;
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
        @apply text-xl font-semibold text-gray-900 mb-4;
    }
    :global(.dark h2) {
        @apply text-white;
    }

    :global(h4) {
        @apply text-lg font-medium text-gray-800 mt-6 mb-3;
    }
    :global(.dark h4) {
        @apply text-white;
    }

    :global(a) {
        @apply text-blue-600 hover:text-blue-800;
    }
    :global(.dark a) {
        @apply text-blue-400 hover:text-blue-300;
    }

    :global(label) {
        @apply block text-sm font-medium text-gray-700 mb-1;
    }
    :global(.dark label) {
        @apply text-white;
    }

    :global(p) {
        @apply mb-4;
    }
</style>
