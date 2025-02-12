<script lang="ts">
  import Sidebar from "$lib/components/Sidebar.svelte";
  import HamburgerButton from "$lib/components/HamburgerButton.svelte";
  import { roomName, mobileMenuOpen } from "$lib/stores";
  import { page } from "$app/stores";
  import { onMount } from "svelte";
  import "../app.css";

  // Get the current page name from the URL
  $: pageName = $page.url.pathname.split('/').pop() || 'Home';
  $: pageTitle = pageName.charAt(0).toUpperCase() + pageName.slice(1);

  // Update title when room name or page changes
  $: if (typeof document !== 'undefined') {
    document.title = `ESPresense ${$roomName ? `(${$roomName})` : ''} - ${pageTitle}`;
  }

  // Close mobile menu when clicking outside
  function handleClickOutside(event: MouseEvent) {
    const target = event.target as HTMLElement;
    if ($mobileMenuOpen && !target.closest('nav') && !target.closest('button')) {
      mobileMenuOpen.set(false);
    }
  }

  // Close mobile menu on route change
  $: if ($page) {
    mobileMenuOpen.set(false);
  }

  // Prevent body scroll when mobile menu is open
  $: if (typeof document !== 'undefined') {
    if ($mobileMenuOpen) {
      document.body.style.overflow = 'hidden';
    } else {
      document.body.style.overflow = '';
    }
  }
</script>

<div class="flex flex-col min-h-screen bg-gray-100 dark:bg-gray-900" on:click={handleClickOutside}>
  <!-- Mobile menu overlay -->
  {#if $mobileMenuOpen}
    <div class="fixed inset-0 bg-gray-600 bg-opacity-75 transition-opacity md:hidden" />
  {/if}

  <!-- Mobile menu button -->
  <div class="fixed top-4 left-4 z-40 md:hidden">
    <HamburgerButton />
  </div>

  <!-- Sidebar -->
  <nav class="fixed md:static inset-y-0 left-0 transform {$mobileMenuOpen ? 'translate-x-0' : '-translate-x-full'} md:translate-x-0 w-72 bg-gray-50 dark:bg-gray-800 border-r border-gray-200 dark:border-gray-700 transition-transform duration-300 ease-in-out z-30">
    <Sidebar />
  </nav>

  <!-- Main content -->
  <main class="flex-1 overflow-auto bg-white dark:bg-gray-900 md:ml-72">
    <div class="max-w-7xl mx-auto px-4 sm:px-6 lg:px-6 py-4 mt-16 md:mt-0">
      <slot />
    </div>
  </main>
</div>

<style>
  /* Remove global overflow hidden - we'll handle this via the store */
</style>
