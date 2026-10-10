<script lang="ts">
    import type { Snippet } from 'svelte';
    import type { Writable } from 'svelte/store';
    import type { ExtraSettings } from '#lib/types.js';

    /**
     * A counted, repeated block of settings: a "Number of …" selector bound to <prefix>_count,
     * then `item(n)` for n = 1..count under an "<title> n:" heading. The firmware registers only
     * the slots in use, but stores whatever the form sends for the others (see Settings::group),
     * so raising the count here and filling in the new slot is a single save.
     */
    interface Props {
        settings: Writable<ExtraSettings | null>;
        /** Setting key prefix, e.g. "led" for led_count and led_<n>_* */
        prefix: string;
        /** Singular heading, e.g. "LED" -> "LED 1:" */
        title: string;
        /** Used in the count label, e.g. "LEDs" -> "Number of LEDs" */
        plural: string;
        max: number;
        /** Count when the firmware reports none (must match the firmware's default) */
        defaultCount: number;
        item: Snippet<[number]>;
    }

    let { settings, prefix, title, plural, max, defaultCount, item }: Props = $props();

    const countKey = $derived(`${prefix}_count`);
    const count = $derived.by(() => {
        const raw = $settings?.values[countKey] ?? $settings?.defaults[countKey] ?? defaultCount;
        const n = Number(raw);
        return Number.isFinite(n) ? Math.min(max, Math.max(0, Math.trunc(n))) : defaultCount;
    });
    const slots = $derived(Array.from({ length: count }, (_, i) => i + 1));
    const choices = $derived(Array.from({ length: max + 1 }, (_, i) => i));

    function setCount(event: Event): void {
        const value = (event.currentTarget as HTMLSelectElement).value;
        settings.update((s) => (s ? { ...s, values: { ...s.values, [countKey]: value } } : s));
    }
</script>

<p>
    <label>
        Number of {plural}:<br />
        <select name={countKey} onchange={setCount}>
            {#each choices as n (n)}
                <option value={String(n)} selected={n === count}>{n}</option>
            {/each}
        </select>
    </label>
</p>
{#each slots as n (n)}
    <h4>{title} {n}:</h4>
    {@render item(n)}
{/each}
