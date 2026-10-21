<script lang="ts">
    import { hardwareSettings } from '#lib/stores.js';

    /**
     * A GPIO field. A disabled pin (-1) shows as an empty field reading "off", and clearing the
     * field disables the pin, so the -1 never shows. The range is the chip's (maxPin).
     *
     * With `name` it posts in the form through a hidden field: blank until edited (the node keeps
     * its default), then the pin, -1 when cleared. Without, `onchange` gets the pin.
     */
    interface Props {
        /** Stored value: a number, a numeric string, or unset */
        value?: unknown;
        /** Shown while value is unset, e.g. the setting's default */
        fallback?: unknown;
        name?: string;
        onchange?: (pin: number) => void;
        class?: string;
    }

    let { value = $bindable(), fallback = -1, name, onchange, class: cls = 'w-24' }: Props = $props();

    const unset = (v: unknown) => v === undefined || v === null || v === '';
    const pin = $derived(Number(unset(value) ? fallback : value));
    const shown = $derived(Number.isInteger(pin) && pin >= 0 ? String(pin) : '');

    function input(text: string): void {
        const p = text === '' ? -1 : Number(text);
        value = p;
        onchange?.(p);
    }
</script>

<input type="number" step="1" min="0" max={$hardwareSettings?.maxPin ?? 48} placeholder="off" class={cls} value={shown}
    oninput={(e) => input(e.currentTarget.value)}/>
{#if name}
    <input type="hidden" {name} value={unset(value) ? '' : String(value)}/>
{/if}
