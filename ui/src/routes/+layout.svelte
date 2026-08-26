<script lang="ts">
    import Sidebar from "$lib/components/Sidebar.svelte";
    import HamburgerButton from "$lib/components/HamburgerButton.svelte";
    import { roomName, mobileMenuOpen } from "$lib/stores";
    import { page } from "$app/stores";
    import { onMount, onDestroy } from "svelte";
    import "../app.css";

    // Get the current page name from the URL
    $: pageName = $page.url.pathname.split("/").pop() || "Home";
    $: pageTitle = pageName.charAt(0).toUpperCase() + pageName.slice(1);

    // Update title when room name or page changes
    $: if (typeof document !== "undefined") {
        document.title = `ESPresense ${$roomName ? `(${$roomName})` : ""} - ${pageTitle}`;
    }

    // Close mobile menu when clicking outside
    function handleClickOutside(event: PointerEvent) {
        const target = event.target as HTMLElement;
        if ($mobileMenuOpen && !target.closest("nav") && !target.closest("button")) {
            mobileMenuOpen.set(false);
        }
    }

    // Close mobile menu on Escape key
    function handleKeyDown(event: KeyboardEvent) {
        if (event.key === "Escape" && $mobileMenuOpen) {
            mobileMenuOpen.set(false);
        }
    }

    // Register window-level keydown listener
    onMount(() => {
        window.addEventListener("keydown", handleKeyDown);
    });

    onDestroy(() => {
        window.removeEventListener("keydown", handleKeyDown);
    });

    // Close mobile menu on route change
    $: if ($page) {
        mobileMenuOpen.set(false);
    }

    // Prevent body scroll when mobile menu is open
    $: if (typeof document !== "undefined") {
        if ($mobileMenuOpen) {
            document.body.style.overflow = "hidden";
        } else {
            document.body.style.overflow = "";
        }
    }
</script>

<!-- svelte-ignore a11y_no_static_element_interactions -->
<div class="flex flex-col md:flex-row h-screen overflow-hidden bg-gray-100 dark:bg-gray-900" on:pointerdown={handleClickOutside}>
    <!-- Mobile menu overlay -->
    {#if $mobileMenuOpen}
        <div class="fixed inset-0 bg-gray-600 bg-opacity-75 transition-opacity md:hidden"></div>
    {/if}

    <!-- Mobile menu button -->
    <div class="fixed top-4 left-4 z-40 md:hidden">
        <HamburgerButton />
    </div>

    <!-- Sidebar -->
    <nav class="fixed md:relative inset-y-0 left-0 transform {$mobileMenuOpen ? 'translate-x-0' : '-translate-x-full'} md:translate-x-0 w-72 bg-gray-50 dark:bg-gray-800 border-r border-gray-200 dark:border-gray-700 transition-transform duration-300 ease-in-out z-30 shrink-0">
        <Sidebar />
    </nav>

    <!-- Main content -->
    <main class="flex-1 min-w-0 overflow-auto bg-white dark:bg-gray-900">
        <div class="max-w-7xl mx-auto px-4 py-4 md:mt-0 mt-14">
            <slot />
        </div>
    </main>
</div>

<style>
    /* Remove global overflow hidden - we'll handle this via the store */
</style>
