<script lang="ts">
    import { base } from "$app/paths";
    import SidebarItem from "./SidebarItem.svelte";
    import DarkModeToggle from "./DarkModeToggle.svelte";
    import Icon from "./Icon.svelte";
</script>

<div class="h-full w-72 bg-gray-50 dark:bg-gray-800 border-r border-gray-200 dark:border-gray-700">
    <div class="flex items-center justify-between mx-6 mt-4">
        <div class="flex items-center flex-shrink-0">
            <Icon name="logo" width="2.5rem" height="2.5rem" />
            <span class="text-gray-800 dark:text-gray-100 ml-2 text-3xl font-thin">ESPresense</span>
        </div>
        <DarkModeToggle />
    </div>
    <nav class="mt-6 px-6">
        <SidebarItem icon="contact" title="Settings" href="{base}/settings" />
        <SidebarItem icon="device" title="Devices" href="{base}/devices" />
        <SidebarItem icon="fingerprint" title="Fingerprints" href="{base}/fingerprints" />
    </nav>
</div>
