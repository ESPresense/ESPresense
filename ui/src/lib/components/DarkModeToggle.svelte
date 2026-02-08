<script lang="ts">
    import { themeMode } from '$lib/stores';
    import Icon from './Icon.svelte';

    const options = [
        { value: 'system', label: 'System', icon: 'system' },
        { value: 'light', label: 'Light', icon: 'sun' },
        { value: 'dark', label: 'Dark', icon: 'moon' }
    ] as const;
</script>

<div
    class="inline-flex items-center rounded-full bg-gray-200 p-1 text-gray-600 dark:bg-gray-700 dark:text-gray-200"
    role="group"
    aria-label="Theme"
>
    {#each options as option}
        <button
            type="button"
            class={`flex items-center gap-1 rounded-full px-3 py-1.5 text-xs font-medium transition ${
                $themeMode === option.value
                    ? 'bg-white text-gray-900 shadow-sm dark:bg-gray-900 dark:text-white'
                    : 'hover:text-gray-900 dark:hover:text-white'
            }`}
            onclick={() => ($themeMode = option.value)}
            aria-pressed={$themeMode === option.value}
            aria-label={`Use ${option.label.toLowerCase()} theme`}
        >
            <Icon name={option.icon} class_="h-4 w-4" />
            <span>{option.label}</span>
        </button>
    {/each}
</div>
