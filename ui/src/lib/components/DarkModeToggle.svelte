<script lang="ts">
    import { darkMode } from '$lib/stores';
    import Icon from './Icon.svelte';
</script>

<button
    class="p-2 rounded-lg text-gray-500 dark:text-gray-400 hover:bg-gray-100 dark:hover:bg-gray-700 focus:outline-hidden focus:ring-2 focus:ring-gray-200 dark:focus:ring-gray-700"
    onclick={() => $darkMode = !$darkMode}
    aria-label="Toggle dark mode"
>
    {#if $darkMode}
        <Icon name="sun" class="w-5 h-5" />
    {:else}
        <Icon name="moon" class="w-5 h-5" />
    {/if}
</button>
