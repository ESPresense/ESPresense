<script lang="ts">
    import { base } from "$app/paths";
    import SidebarItem from "./SidebarItem.svelte";
    import DarkModeToggle from "./DarkModeToggle.svelte";
    import Icon from "./Icon.svelte";
    import { firmwareInfo } from "$lib/stores";
</script>

<div class="h-full w-full flex flex-col">
    <div class="mx-6 mt-4 pt-14 md:pt-4">
        <div class="flex items-center shrink-0">
            <Icon name="logo" width="2.5rem" height="2.5rem" />
            <span class="text-gray-800 dark:text-gray-100 ml-2 text-3xl font-thin">ESPresense</span>
        </div>
    </div>
    <nav class="mt-6 px-6 space-y-1 flex-1">
        <SidebarItem icon="network" title="Network" href="{base}/network" />
        <SidebarItem icon="contact" title="Settings" href="{base}/settings" />
        <SidebarItem icon="hardware" title="Hardware" href="{base}/hardware" />
        <SidebarItem icon="device" title="Devices" href="{base}/devices" />
        <SidebarItem icon="fingerprint" title="Fingerprints" href="{base}/fingerprints" />
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
