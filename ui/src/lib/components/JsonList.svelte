<script lang="ts">
    import type { Snippet } from 'svelte';
    import type { Writable } from 'svelte/store';
    import type { ExtraSettings } from '#lib/types.js';

    type Item = Record<string, unknown>;

    /**
     * A setting that is a JSON list of objects (inputs, outputs): one "<title> n:" block per item
     * with a Remove button, an Add button up to `max`, and a hidden field posting the list as JSON.
     */
    interface Props {
        settings: Writable<ExtraSettings | null>;
        /** Setting name, e.g. "outputs" */
        key: string;
        /** Singular heading, e.g. "Output" -> "Output 1:" */
        title: string;
        max: number;
        /** A new item's fields */
        blank: () => Item;
        /** Renders item i's fields; set(field, value) updates it */
        item: Snippet<[Item, number, (field: string, value: unknown) => void]>;
    }

    let { settings, key, title, max, blank, item }: Props = $props();

    const items = $derived(($settings?.values[key] ?? $settings?.defaults[key] ?? []) as Item[]);

    function save(list: Item[]): void {
        settings.update((s) => (s ? { ...s, values: { ...s.values, [key]: list } } : s));
    }
    const setter = (i: number) => (field: string, value: unknown) =>
        save(items.map((it, j) => (j === i ? { ...it, [field]: value } : it)));
</script>

<input type="hidden" name={key} value={JSON.stringify(items)} />
{#each items as it, i (i)}
    <h4 class="flex items-center gap-4">
        {title} {i + 1}:
        <button type="button" class="text-sm underline" onclick={() => save(items.filter((_, j) => j !== i))}>Remove</button>
    </h4>
    {@render item(it, i, setter(i))}
{/each}
{#if items.length < max}
    <p>
        <button type="button" class="underline" onclick={() => save([...items, blank()])}>Add {title.toLowerCase()}</button>
    </p>
{/if}
