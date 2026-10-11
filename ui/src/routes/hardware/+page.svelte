<script lang="ts">
    import { hardwareSettings } from '#lib/stores.js';
    import { saveSettingsWithRetry } from '#lib/utils/settings.js';
    import JsonList from '#lib/components/JsonList.svelte';

    const ledTypes = [
        ['pwm', 'PWM'], ['pwm_inverted', 'PWM Inverted'],
        ['grb', 'Addressable GRB'], ['grbw', 'Addressable GRBW'], ['rgb', 'Addressable RGB'], ['rgbw', 'Addressable RGBW']
    ];
    const ledControls = [['mqtt', 'MQTT'], ['status', 'Status'], ['motion', 'Motion'], ['count', 'Count'], ['output', 'Output']];
    const powerModels = [['', 'None'], ['bl0937', 'BL0937'], ['hlw8012', 'HLW8012 / CSE7759'], ['cse7766', 'CSE7766 (serial, e.g. Athom)']];
    // [stored value, label] for the inputs/outputs JSON lists.
    const inputRoles = [['motion', 'Motion'], ['switch', 'Switch'], ['button', 'Button']];
    const inputPinTypes = [
        ['pullup', 'Pullup'], ['pullup_inverted', 'Pullup Inverted'],
        ['pulldown', 'Pulldown'], ['pulldown_inverted', 'Pulldown Inverted'],
        ['floating', 'Floating'], ['floating_inverted', 'Floating Inverted']
    ];
    const outputPinTypes = [['output', 'Output'], ['output_inverted', 'Output Inverted'], ['high', 'Always High'], ['low', 'Always Low']];
    const powerOnStates = [['off', 'Off'], ['on', 'On'], ['restore', 'Restore last']];

    // "Linked input" choices, labelled with each input's name (value = 1-based input number, 0 = none).
    // Outputs an LED can mirror (value = 1-based output number).
    const outputChoices = $derived(
        (($hardwareSettings?.values['outputs'] ?? []) as Record<string, unknown>[]).map((it, i) => [
            String(i + 1),
            `${i + 1}: ${it.name || `Output ${i + 1}`}`
        ])
    );

    // The "power" setting is one JSON object, posted as a hidden field.
    const power = $derived(($hardwareSettings?.values['power'] ?? $hardwareSettings?.defaults['power'] ?? {}) as Record<string, unknown>);
    function setPower(field: string, value: unknown): void {
        hardwareSettings.update((s) => (s ? { ...s, values: { ...s.values, power: { ...power, [field]: value } } } : s));
    }

    const linkedInputs = $derived([
        ['0', 'None'],
        ...((($hardwareSettings?.values['inputs'] ?? []) as Record<string, unknown>[]).map((it, i) => [
            String(i + 1),
            `${i + 1}: ${it.name || `Input ${i + 1}`}`
        ]))
    ]);

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

<!-- Fields of one inputs/outputs list item. They have no form name: JsonList posts the list. -->
{#snippet field(label: string, value: unknown, set: (v: string) => void, placeholder = '')}
    <p>
        <label>
            {label}:<br />
            <input type="text" value={value ?? ''} {placeholder} oninput={(e) => set(e.currentTarget.value)}/>
        </label>
    </p>
{/snippet}

{#snippet choose(label: string, value: unknown, options: string[][], set: (v: string) => void)}
    <p>
        <label>
            {label}:<br />
            <select value={String(value ?? options[0][0])} onchange={(e) => set(e.currentTarget.value)}>
                {#each options as [v, text] (v)}
                    <option value={v} selected={String(value ?? options[0][0]) === v}>{text}</option>
                {/each}
            </select>
        </label>
    </p>
{/snippet}

{#snippet itemPin(it: Record<string, unknown>, set: (field: string, value: unknown) => void, types: string[][])}
    <p>
        <label>
            Pin (-1 to disable) and type:<br />
            <span class="flex items-center gap-2">
                <input
                    type="number"
                    step="1"
                    min="-1"
                    max="48"
                    class="w-24"
                    value={it.pin ?? -1}
                    oninput={(e) => set('pin', e.currentTarget.value === '' ? -1 : Number(e.currentTarget.value))}/>
                <select aria-label="Pin type" onchange={(e) => set('type', e.currentTarget.value)}>
                    {#each types as [v, text] (v)}
                        <option value={v} selected={(it.type ?? types[0][0]) === v}>{text}</option>
                    {/each}
                </select>
            </span>
        </label>
    </p>
{/snippet}

{#snippet led(it: Record<string, unknown>, i: number, set: (field: string, value: unknown) => void)}
    {@render itemPin(it, set, ledTypes)}
    {#if it.type !== 'pwm' && it.type !== 'pwm_inverted' && it.type !== undefined}
        <p>
            <label>
                Count:<br />
                <input type="number" step="1" min="1" max="1000" value={it.count ?? 1}
                    oninput={(e) => set('count', Number(e.currentTarget.value))}/>
            </label>
        </p>
    {/if}
    <p>
        <label>
            Max brightness (1-255):<br />
            <input type="number" step="1" min="1" max="255"
                value={it.max_brightness ?? (it.type === 'pwm' || it.type === 'pwm_inverted' || it.type === undefined ? 255 : 100)}
                oninput={(e) => set('max_brightness', Number(e.currentTarget.value))}/>
        </label>
    </p>
    {@render choose('LED Control', it.control, ledControls, (v) => set('control', v))}
    {#if it.control === 'output'}
        {@render choose('Output to show', String(it.output ?? 1), outputChoices.length ? outputChoices : [['1', 'Output 1']], (v) => set('output', Number(v)))}
    {/if}
{/snippet}

{#snippet input(it: Record<string, unknown>, i: number, set: (field: string, value: unknown) => void)}
    {@render field('Name', it.name, (v) => set('name', v), `Input ${i + 1}`)}
    {@render choose('Role', it.role, inputRoles, (v) => set('role', v))}
    {@render itemPin(it, set, inputPinTypes)}
    <p>
        <label>
            Timeout (in seconds):<br />
            <input type="number" step="0.01" min="0" max="300" value={it.timeout ?? 0.5}
                oninput={(e) => set('timeout', Number(e.currentTarget.value))}/>
        </label>
    </p>
{/snippet}

{#snippet output(it: Record<string, unknown>, i: number, set: (field: string, value: unknown) => void)}
    {@render field('Name', it.name, (v) => set('name', v), `Output ${i + 1}`)}
    {@render itemPin(it, set, outputPinTypes)}
    {#if it.type !== 'high' && it.type !== 'low'}
        {@render choose('Power-on state', it.power_on, powerOnStates, (v) => set('power_on', v))}
        {@render choose('Linked input (a Button toggles it, a Switch or Motion input drives it)', String(it.input ?? 0), linkedInputs, (v) => set('input', Number(v)))}
    {/if}
{/snippet}

<div class="bg-gray-100 dark:bg-gray-800 rounded-lg shadow p-6">
    {#if $hardwareSettings?.values != null}
    <form action="wifi/hardware" method="post" id="hardware" onsubmit={handleSubmit} class="space-y-6">
        <h2>
            <a href="https://espresense.com/configuration/settings#leds" target="_blank">LEDs</a>
        </h2>
        <JsonList settings={hardwareSettings} key="leds" title="LED" max={4} blank={() => ({ type: 'pwm', pin: -1, control: 'mqtt' })} item={led} />
        <h2>
            <a href="https://espresense.com/configuration/settings#inputs" target="_blank">Inputs</a>
        </h2>
        <JsonList settings={hardwareSettings} key="inputs" title="Input" max={8} blank={() => ({ name: '', role: 'motion', pin: -1, type: 'pullup', timeout: 0.5 })} item={input} />
        <h2>
            <a href="https://espresense.com/configuration/settings#outputs" target="_blank">Outputs</a>
        </h2>
        <JsonList settings={hardwareSettings} key="outputs" title="Output" max={8} blank={() => ({ name: '', pin: -1, type: 'output', power_on: 'off', input: 0 })} item={output} />
        <h2>
            <a href="https://espresense.com/configuration/settings#power" target="_blank">Power</a>
        </h2>
        <h4>Battery:</h4>
        <p>
            <label>
                <input type="checkbox" name="axp192" bind:checked={$hardwareSettings.values['axp192']}/>
                <span>AXP192 power chip (M5StickC): reads the battery from it instead of a pin. Needs I2C bus 2 on SDA 21 / SCL 22.</span>
            </label>
        </p>
        <div class="flex flex-wrap gap-4">
            <p>
                <label>
                    Voltage pin (-1 to disable, ADC1 only):<br />
                    <input type="number" step="1" min="-1" max="48" name="batt_pin"
                        placeholder={$hardwareSettings.defaults['batt_pin']}
                        bind:value={$hardwareSettings.values['batt_pin']}/>
                </label>
            </p>
            <p>
                <label>
                    Divider multiplier:<br />
                    <input type="number" step="0.01" min="1" max="20" name="batt_mult"
                        placeholder={$hardwareSettings.defaults['batt_mult']}
                        bind:value={$hardwareSettings.values['batt_mult']}/>
                </label>
            </p>
            <p>
                <label>
                    Type:<br />
                    <select name="batt_type" bind:value={$hardwareSettings.values['batt_type']}>
                        <option value="0">Li-ion (1 cell)</option>
                        <option value="1">12V lead-acid</option>
                    </select>
                </label>
            </p>
        </div>
        <h4>Energy meter:</h4>
        <input type="hidden" name="power" value={JSON.stringify(power)} />
        {@render choose('Chip (most relay plugs have one)', power.model ?? '', powerModels, (v) => setPower('model', v))}
        {#if power.model === 'cse7766'}
            <p>
                <label>
                    RX pin (CSE7766 TX; factory calibrated, no settings needed):<br />
                    <input type="number" step="1" min="-1" max="48" class="w-24" value={power.rx ?? -1}
                        oninput={(e) => setPower('rx', e.currentTarget.value === '' ? -1 : Number(e.currentTarget.value))}/>
                </label>
            </p>
        {:else if power.model}
            <div class="flex flex-wrap gap-4">
                {#each [['cf', 'CF pin (power)'], ['cf1', 'CF1 pin (V / A)'], ['sel', 'SEL pin']] as [key, label] (key)}
                    <p>
                        <label>
                            {label}:<br />
                            <input type="number" step="1" min="-1" max="48" class="w-24" value={power[key] ?? -1}
                                oninput={(e) => setPower(key, e.currentTarget.value === '' ? -1 : Number(e.currentTarget.value))}/>
                        </label>
                    </p>
                {/each}
                <p>
                    <label>
                        SEL polarity:<br />
                        <select onchange={(e) => setPower('sel_inverted', e.currentTarget.value === '1')}>
                            <option value="0" selected={!power.sel_inverted}>Normal</option>
                            <option value="1" selected={!!power.sel_inverted}>Inverted</option>
                        </select>
                    </label>
                </p>
            </div>
            <div class="flex flex-wrap gap-4">
                <p>
                    <label>
                        Voltage divider (as in ESPHome's hlw8012):<br />
                        <input type="number" step="any" min="1" value={power.voltage_divider ?? 2351}
                            oninput={(e) => setPower('voltage_divider', Number(e.currentTarget.value))}/>
                    </label>
                </p>
                <p>
                    <label>
                        Current resistor in ohms (as in ESPHome's hlw8012):<br />
                        <input type="number" step="any" min="0" value={power.current_resistor ?? 0.001}
                            oninput={(e) => setPower('current_resistor', Number(e.currentTarget.value))}/>
                    </label>
                </p>
            </div>
        {/if}
        <h2>
            <a href="https://espresense.com/configuration/settings#gpio-sensors" target="_blank">GPIO Sensors</a>
        </h2>
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
