<script lang="ts">
    import { hardwareSettings } from '#lib/stores.js';

    /**
     * A GPIO picker: Off, or the GPIOs the node says this setting can use (pins: no flash/PSRAM
     * pins, and no input-only ones unless `input`). Never shows the -1 that stores "off".
     *
     * With `name` it posts in the form and starts with "Default (...)": unset, so the node follows
     * the setting's default; choosing it again unsets the pin. Without, `onchange` gets the pin.
     */
    interface Props {
        /** Stored value: a number, a numeric string, or unset */
        value?: unknown;
        /** The setting's default, shown in the "Default" option */
        fallback?: unknown;
        name?: string;
        label?: string;
        /** The firmware only reads this pin, so input-only GPIOs are fine */
        input?: boolean;
        /** The firmware reads this pin through ADC1 (e.g. a battery): only the node's ADC1 pins */
        analog?: boolean;
        onchange?: (pin: number) => void;
        class?: string;
    }

    let { value = $bindable(), fallback = -1, name, label, input = false, analog = false, onchange, class: cls = 'w-auto' }: Props = $props();

    const unset = (v: unknown) => v === undefined || v === null || v === '';
    const describe = (p: number) => (p >= 0 ? `GPIO ${p}` : 'off');
    // Older firmware sends no list: offer 0..48 and let the node's save check refuse a bad one.
    const usable = $derived.by(() => {
        const pins = $hardwareSettings?.pins;
        if (!pins) return Array.from({ length: 49 }, (_, i) => i);
        // Firmware before pins.adc1 sends no list; then an analog pin is offered like any input.
        if (analog && pins.adc1) return [...pins.adc1].sort((a, b) => a - b);
        return pins.usable.filter((p) => input || analog || !pins.inputOnly.includes(p)).sort((a, b) => a - b);
    });
    // A stored pin the list doesn't offer (e.g. set on another chip) stays shown, marked, rather
    // than silently changing.
    const stray = $derived.by(() => {
        const current = Number(value);
        return !unset(value) && Number.isInteger(current) && current >= 0 && !usable.includes(current) ? current : undefined;
    });
    const selected = $derived(unset(value) ? (name ? '' : '-1') : String(Number(value) < 0 ? -1 : Number(value)));

    function choose(v: string): void {
        if (v === '') {
            value = '';
            return;
        }
        value = Number(v);
        onchange?.(Number(v));
    }
</script>

<select {name} aria-label={label} class={cls} onchange={(e) => choose(e.currentTarget.value)}>
    {#if name}
        <option value="" selected={selected === ''}>Default ({describe(unset(fallback) ? -1 : Number(fallback))})</option>
    {/if}
    <option value="-1" selected={selected === '-1'}>Off</option>
    {#each usable as p (p)}
        <option value={String(p)} selected={selected === String(p)}>GPIO {p}</option>
    {/each}
    {#if stray !== undefined}
        <option value={String(stray)} selected>GPIO {stray} (not usable here)</option>
    {/if}
</select>
