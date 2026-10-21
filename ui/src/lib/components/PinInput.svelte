<script lang="ts">
    import { hardwareSettings } from '#lib/stores.js';

    /**
     * A GPIO picker: Off, or GPIO 0 up to the chip's highest (maxPin). Never shows the -1 that
     * stores "off".
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
        onchange?: (pin: number) => void;
        class?: string;
    }

    let { value = $bindable(), fallback = -1, name, label, onchange, class: cls = 'w-32' }: Props = $props();

    const unset = (v: unknown) => v === undefined || v === null || v === '';
    const describe = (p: number) => (p >= 0 ? `GPIO ${p}` : 'off');
    // A stored pin past maxPin (e.g. from another chip) stays listed rather than silently changing.
    const pins = $derived.by(() => {
        const max = $hardwareSettings?.maxPin ?? 48;
        const list = Array.from({ length: max + 1 }, (_, i) => i);
        const current = Number(value);
        if (!unset(value) && Number.isInteger(current) && current > max) list.push(current);
        return list;
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
    {#each pins as p (p)}
        <option value={String(p)} selected={selected === String(p)}>GPIO {p}</option>
    {/each}
</select>
