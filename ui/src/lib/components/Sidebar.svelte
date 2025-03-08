<script lang="ts">
    import { base } from "$app/paths";
    import SidebarItem from "./SidebarItem.svelte";
    import DarkModeToggle from "./DarkModeToggle.svelte";
    import Icon from "./Icon.svelte";
    import { _ } from "../../lang/i18n";
</script>

<div class="h-full w-full">
    <div class="flex items-center justify-between mx-6 mt-4 pt-14 md:pt-4">
        <div class="flex items-center flex-shrink-0">
            <Icon name="logo" width="2.5rem" height="2.5rem" />
            <span class="text-gray-800 dark:text-gray-100 ml-2 text-3xl font-thin">ESPresense</span>
        </div>
        <DarkModeToggle />
    </div>
    <nav class="mt-6 px-6 space-y-1">
        <SidebarItem icon="network" title={$_('sidebar.network')} href="{base}/network" />
        <SidebarItem icon="contact" title={$_('sidebar.settings')} href="{base}/settings" />
        <SidebarItem icon="device" title={$_('sidebar.devices')} href="{base}/devices" />
        <SidebarItem icon="fingerprint" title={$_('sidebar.fingerprints')} href="{base}/fingerprints" />
    </nav>
</div>
