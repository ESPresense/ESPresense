<script lang="ts">
    import { resolve } from "$app/paths";
    import SidebarItem from "./SidebarItem.svelte";
    import DarkModeToggle from "./DarkModeToggle.svelte";
    import Icon from "./Icon.svelte";
    import { firmwareInfo, palette } from "#lib/stores.js";

    // navigator.platform is deprecated; the user agent names the OS just as well for a key hint.
    const mac = typeof navigator !== "undefined" && /Mac|iPhone|iPad/.test(navigator.userAgent);
</script>

<div class="h-full w-full flex flex-col">
    <div class="mx-6 mt-4 pt-14 md:pt-4">
        <div class="flex items-center shrink-0">
            <Icon name="logo" width="2.5rem" height="2.5rem" />
            <span class="text-gray-800 dark:text-gray-100 ml-2 text-3xl font-thin">ESPresense</span>
        </div>
    </div>
    <div class="mx-6 mt-6">
        <button
            type="button"
            onclick={() => palette.set({ view: "search" })}
            class="flex w-full items-center gap-2 rounded-lg border border-gray-300 bg-white px-3 py-2 text-sm text-gray-500 hover:border-gray-400 dark:border-gray-600 dark:bg-gray-900 dark:text-gray-400 dark:hover:border-gray-500"
        >
            <svg class="h-4 w-4" viewBox="0 0 20 20" fill="currentColor" aria-hidden="true"><path fill-rule="evenodd" d="M9 3.5a5.5 5.5 0 1 0 0 11 5.5 5.5 0 0 0 0-11ZM2 9a7 7 0 1 1 12.45 4.39l3.08 3.08a.75.75 0 1 1-1.06 1.06l-3.08-3.08A7 7 0 0 1 2 9Z" clip-rule="evenodd" /></svg>
            <span>Search or find…</span>
            <kbd class="ml-auto hidden md:block rounded border border-gray-300 px-1.5 text-xs dark:border-gray-600">{mac ? "⌘" : "Ctrl "}K</kbd>
        </button>
    </div>
    <nav class="mt-3 px-6 flex-1">
        <p class="px-3 pb-1 text-[10px] font-semibold uppercase tracking-[0.16em] text-gray-400 dark:text-gray-500">Configure</p>
        <SidebarItem
            icon="network"
            title="Network"
            href={resolve('/network')}
        />

        <SidebarItem
            icon="scan"
            title="Scanning"
            href={resolve('/settings')}
        />

        <SidebarItem
            icon="hardware"
            title="Hardware"
            href={resolve('/hardware')}
        />

        <SidebarItem
            icon="template"
            title="Templates"
            href={resolve('/templates')}
        />

        <p class="mt-5 px-3 pb-1 text-[10px] font-semibold uppercase tracking-[0.16em] text-gray-400 dark:text-gray-500">Monitor</p>
        <SidebarItem
            icon="device"
            title="Devices"
            href={resolve('/devices')}
        />

        <SidebarItem
            icon="fingerprint"
            title="Fingerprints"
            href={resolve('/fingerprints')}
        />
    </nav>
    <div class="px-6 pb-2">
        <DarkModeToggle />
    </div>
    {#if $firmwareInfo.ver}
        <div class="px-6 pb-4 text-xs text-gray-400 dark:text-gray-500">
            {$firmwareInfo.ver}{#if $firmwareInfo.firm} · {$firmwareInfo.firm}{/if}
        </div>
    {/if}
</div>
