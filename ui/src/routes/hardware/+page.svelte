<script lang="ts">
    import { hardwareSettings } from '#lib/stores.js';
    import { saveSettingsWithRetry } from '#lib/utils/settings.js';
    import RepeatGroup from '#lib/components/RepeatGroup.svelte';

    const ledTypes = ['PWM', 'PWM Inverted', 'Addressable GRB', 'Addressable GRBW', 'Addressable RGB', 'Addressable RGBW'];
    const ledControls = ['MQTT', 'Status', 'Motion', 'Count'];
    const relayTypes = ['Output', 'Output Inverted'];
    const powerOnStates = ['Off', 'On', 'Restore last'];
    const relayButtons = ['None', 'Button 1', 'Button 2', 'Button 3', 'Button 4'];
    const pinTypes = ['Pullup', 'Pullup Inverted', 'Pulldown', 'Pulldown Inverted', 'Floating', 'Floating Inverted'];

    /** Tracks whether the form is currently being saved */
    let isSaving = $state<boolean>(false);

    /**
     * Handles form submission for hardware settings.
     * Saves settings to the device and triggers a restart with automatic retry.
     */
    async function handleSubmit(event: SubmitEvent): Promise<void> {
        try {
            event.preventDefault();
            isSaving = true;
            const form = event.target as HTMLFormElement;
            const formData = new FormData(form);
            await saveSettingsWithRetry('/wifi/hardware', formData, hardwareSettings);
        } catch (error) {
            console.error('Failed to save hardware settings:', error);
            // TODO: Show user-friendly error toast notification
        } finally {
            isSaving = false;
        }
    }
</script>

{#snippet dropdown(name: string, label: string, options: string[])}
    <p>
        <label>
            {label}:<br />
            <select {name} bind:value={$hardwareSettings!.values[name]}>
                <option disabled selected hidden>{options[0]}</option>
                {#each options as option, i (i)}
                    <option value={String(i)}>{option}</option>
                {/each}
            </select>
        </label>
    </p>
{/snippet}

{#snippet number(name: string, label: string, attrs: { step: string; min?: string; max?: string })}
    <p>
        <label>
            {label}:<br />
            <input
                type="number"
                {...attrs}
                {name}
                placeholder={$hardwareSettings!.defaults[name]}
                bind:value={$hardwareSettings!.values[name]}/>
        </label>
    </p>
{/snippet}

{#snippet led(n: number)}
    {@render dropdown(`led_${n}_type`, 'LED Type', ledTypes)}
    {@render number(`led_${n}_pin`, 'Pin (-1 to disable)', { step: '1', min: '-1', max: '48' })}
    {@render number(`led_${n}_cnt`, 'Count (only applies to Addressable LEDs)', { step: '1', min: '-1', max: '39' })}
    {@render dropdown(`led_${n}_cntrl`, 'LED Control', ledControls)}
{/snippet}

{#snippet relay(n: number)}
    {@render dropdown(`relay_${n}_type`, 'Relay pin type', relayTypes)}
    {@render number(`relay_${n}_pin`, 'Pin (-1 to disable)', { step: '1', min: '-1', max: '48' })}
    {@render dropdown(`relay_${n}_state`, 'Power-on state', powerOnStates)}
    {@render dropdown(`relay_${n}_button`, 'Toggle with button', relayButtons)}
{/snippet}

{#snippet gpioInput(prefix: string, label: string, n: number)}
    {@render dropdown(`${prefix}_${n}_type`, `${label} ${n} pin type`, pinTypes)}
    {@render number(`${prefix}_${n}_pin`, `${label} ${n} pin (-1 for disable)`, { step: '1' })}
    {@render number(`${prefix}_${n}_timeout`, `${label} ${n} timeout (in seconds)`, { step: '0.01', min: '0', max: '300' })}
{/snippet}

{#snippet switchItem(n: number)}
    {@render gpioInput('switch', 'Switch', n)}
{/snippet}

{#snippet buttonItem(n: number)}
    {@render gpioInput('button', 'Button', n)}
{/snippet}

<div class="bg-gray-100 dark:bg-gray-800 rounded-lg shadow p-6">
    {#if $hardwareSettings?.values != null}
    <form action="wifi/hardware" method="post" id="hardware" onsubmit={handleSubmit} class="space-y-6">
        <h2>
            <a href="https://espresense.com/configuration/settings#leds" target="_blank">LEDs</a>
        </h2>
        <RepeatGroup settings={hardwareSettings} prefix="led" title="LED" plural="LEDs" max={4} defaultCount={3} item={led} />
        <h4>LED power:</h4>
        <p>
            <label>
                LED power pin (-1 to disable), held high so the LEDs get power:<br />
                <input
                    type="number"
                    step="1"
                    min="-1"
                    max="48"
                    name="led_pwr_pin"
                    placeholder={$hardwareSettings.defaults['led_pwr_pin']}
                    bind:value={$hardwareSettings.values['led_pwr_pin']}/>
            </label>
        </p>
        <h2>
            <a href="https://espresense.com/configuration/settings#relays" target="_blank">Relays</a>
        </h2>
        <RepeatGroup settings={hardwareSettings} prefix="relay" title="Relay" plural="relays" max={4} defaultCount={0} item={relay} />
        <h2>
            <a href="https://espresense.com/configuration/settings#gpio-sensors" target="_blank">GPIO Sensors</a>
        </h2>
        <h4>PIR:</h4>
        <p>
            <label>
                PIR motion pin type:<br />
                <select name="pir_type" bind:value={$hardwareSettings.values['pir_type']}>
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
                    placeholder={$hardwareSettings.defaults['pir_pin']}
                    bind:value={$hardwareSettings.values['pir_pin']}/>
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
                    placeholder={$hardwareSettings.defaults['pir_timeout']}
                    bind:value={$hardwareSettings.values['pir_timeout']}/>
            </label>
        </p>
        <h4>Radar:</h4>
        <p>
            <label>
                Radar motion pin type:<br />
                <select name="radar_type" bind:value={$hardwareSettings.values['radar_type']}>
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
                    placeholder={$hardwareSettings.defaults['radar_pin']}
                    bind:value={$hardwareSettings.values['radar_pin']}/>
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
                    placeholder={$hardwareSettings.defaults['radar_timeout']}
                    bind:value={$hardwareSettings.values['radar_timeout']}/>
            </label>
        </p>
        <RepeatGroup settings={hardwareSettings} prefix="switch" title="Switch" plural="switches" max={4} defaultCount={2} item={switchItem} />
        <RepeatGroup settings={hardwareSettings} prefix="button" title="Button" plural="buttons" max={4} defaultCount={2} item={buttonItem} />
        <h4>DHT:</h4>
        <p>
            <label>
                DHT11 sensor pin (-1 for disable):<br />
                <input
                    type="number"
                    step="1"
                    name="dht11_pin"
                    placeholder={$hardwareSettings.defaults['dht11_pin']}
                    bind:value={$hardwareSettings.values['dht11_pin']}/>
            </label>
        </p>
        <p>
            <label>
                DHT22 sensor pin (-1 for disable):<br />
                <input
                    type="number"
                    step="1"
                    name="dht22_pin"
                    placeholder={$hardwareSettings.defaults['dht22_pin']}
                    bind:value={$hardwareSettings.values['dht22_pin']}/>
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
                    placeholder={$hardwareSettings.defaults['dhtTemp_offset']}
                    bind:value={$hardwareSettings.values['dhtTemp_offset']}/>
            </label>
        </p>
        <p>
            <label>
                DHT humidity offset:<br />
                <input
                    type="number"
                    step="0.01"
                    min="-100"
                    max="100"
                    name="dhtHumidity_offset"
                    placeholder={$hardwareSettings.defaults['dhtHumidity_offset']}
                    bind:value={$hardwareSettings.values['dhtHumidity_offset']}/>
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
                    max="48"
                    name="I2C_Bus_1_SDA"
                    placeholder={$hardwareSettings.defaults['I2C_Bus_1_SDA']}
                    bind:value={$hardwareSettings.values['I2C_Bus_1_SDA']}/>
            </label>
        </p>
        <p>
            <label>
                SCL pin (-1 to disable):<br />
                <input
                    type="number"
                    step="1"
                    min="-1"
                    max="48"
                    name="I2C_Bus_1_SCL"
                    placeholder={$hardwareSettings.defaults['I2C_Bus_1_SCL']}
                    bind:value={$hardwareSettings.values['I2C_Bus_1_SCL']}/>
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
                    max="48"
                    name="I2C_Bus_2_SDA"
                    placeholder={$hardwareSettings.defaults['I2C_Bus_2_SDA']}
                    bind:value={$hardwareSettings.values['I2C_Bus_2_SDA']}/>
            </label>
        </p>
        <p>
            <label>
                SCL pin (-1 to disable):<br />
                <input
                    type="number"
                    step="1"
                    min="-1"
                    max="48"
                    name="I2C_Bus_2_SCL"
                    placeholder={$hardwareSettings.defaults['I2C_Bus_2_SCL']}
                    bind:value={$hardwareSettings.values['I2C_Bus_2_SCL']}/>
            </label>
        </p>
        <p>
            <label class="flex items-center space-x-2">
                <input type="checkbox" name="I2CDebug" bind:checked={$hardwareSettings.values['I2CDebug']}/>
                <span>Debug I2C addresses. Look at the serial log to get the correct address (default: &#x2610;)</span>
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
                    placeholder={$hardwareSettings.defaults['AHTX0_I2c_Bus']}
                    bind:value={$hardwareSettings.values['AHTX0_I2c_Bus']}/>
            </label>
        </p>
        <p>
            <label>
                I2C address (0x38 or 0x39):<br />
                <input name="AHTX0_I2c" bind:value={$hardwareSettings.values['AHTX0_I2c']}/>
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
                    placeholder={$hardwareSettings.defaults['BH1750_I2c_Bus']}
                    bind:value={$hardwareSettings.values['BH1750_I2c_Bus']}/>
            </label>
        </p>
        <p>
            <label>
                I2C address (0x23 or 0x5C):<br />
                <input name="BH1750_I2c" bind:value={$hardwareSettings.values['BH1750_I2c']}/>
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
                    placeholder={$hardwareSettings.defaults['BME280_I2c_Bus']}
                    bind:value={$hardwareSettings.values['BME280_I2c_Bus']}/>
            </label>
        </p>
        <p>
            <label>
                I2C address (0x76 or 0x77):<br />
                <input name="BME280_I2c" bind:value={$hardwareSettings.values['BME280_I2c']}/>
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
                    placeholder={$hardwareSettings.defaults['BMP180_I2c_Bus']}
                    bind:value={$hardwareSettings.values['BMP180_I2c_Bus']}/>
            </label>
        </p>
        <p>
            <label>
                I2C address (0x77):<br />
                <input name="BMP180_I2c" bind:value={$hardwareSettings.values['BMP180_I2c']}/>
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
                    placeholder={$hardwareSettings.defaults['BMP280_I2c_Bus']}
                    bind:value={$hardwareSettings.values['BMP280_I2c_Bus']}/>
            </label>
        </p>
        <p>
            <label>
                I2C address (0x76 or 0x77):<br />
                <input name="BMP280_I2c" bind:value={$hardwareSettings.values['BMP280_I2c']}/>
            </label>
        </p>
        <h4>SHTC1/3, SHTW1/2, SHT2x/3x/4x, SHT85, HTU21D, Si7021 (GY-21) - Temperature and Humidity Sensor:</h4>
        <p>
            <label>
                I2C Bus (-1 to disable):<br />
                <input
                    type="number"
                    step="1"
                    min="-1"
                    max="2"
                    name="SHT_I2c_Bus"
                    placeholder={$hardwareSettings.defaults['SHT_I2c_Bus']}
                    bind:value={$hardwareSettings.values['SHT_I2c_Bus']}/>
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
                    placeholder={$hardwareSettings.defaults['TSL2561_I2c_Bus']}
                    bind:value={$hardwareSettings.values['TSL2561_I2c_Bus']}/>
            </label>
        </p>
        <p>
            <label>
                I2C address (0x39, 0x49 or 0x29):<br />
                <input name="TSL2561_I2c" bind:value={$hardwareSettings.values['TSL2561_I2c']}/>
            </label>
        </p>
        <p>
            <label>
                Gain (auto, 1x or 16x):<br />
                <input
                    name="TSL2561_I2c_Gain"
                    placeholder={$hardwareSettings.defaults['TSL2561_I2c_Gain']}
                    bind:value={$hardwareSettings.values['TSL2561_I2c_Gain']}/>
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
                    placeholder={$hardwareSettings.defaults['SGP30_I2c_Bus']}
                    bind:value={$hardwareSettings.values['SGP30_I2c_Bus']}/>
            </label>
        </p>
        <p>
            <label>
                I2C address (0x58):<br />
                <input name="SGP30_I2c" bind:value={$hardwareSettings.values['SGP30_I2c']}/>
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
                    placeholder={$hardwareSettings.defaults['SCD4x_I2c_Bus']}
                    bind:value={$hardwareSettings.values['SCD4x_I2c_Bus']}/>
            </label>
        </p>
        <p>
            <label>
                I2C address (0x62):<br />
                <input name="SCD4x_I2c" bind:value={$hardwareSettings.values['SCD4x_I2c']}/>
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
                    placeholder={$hardwareSettings.defaults['HX711_sckPin']}
                    bind:value={$hardwareSettings.values['HX711_sckPin']}/>
            </label>
        </p>
        <p>
            <label>
                HX711 DOUT (Data) pin:<br />
                <input
                    type="number"
                    step="1"
                    name="HX711_doutPin"
                    placeholder={$hardwareSettings.defaults['HX711_doutPin']}
                    bind:value={$hardwareSettings.values['HX711_doutPin']}/>
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
                    placeholder={$hardwareSettings.defaults['ds18b20_pin']}
                    bind:value={$hardwareSettings.values['ds18b20_pin']}/>
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
                    placeholder={$hardwareSettings.defaults['dsTemp_offset']}
                    bind:value={$hardwareSettings.values['dsTemp_offset']}/>
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
