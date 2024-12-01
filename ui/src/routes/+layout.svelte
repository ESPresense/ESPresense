<script lang="ts">
  import Sidebar from "$lib/components/Sidebar.svelte";
  import "../app.css";
</script>

<div class="flex h-screen bg-gray-100 dark:bg-gray-900">
  <nav class="flex-none w-72 bg-gray-50 dark:bg-gray-800 border-r border-gray-200 dark:border-gray-700">
    <Sidebar />
  </nav>
  <main class="flex-1 overflow-auto bg-white dark:bg-gray-900">
    <div class="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 py-4">
      <slot />
    </div>
  </main>
</div>

<style>
  :global(body) {
    overflow: hidden;
  }
</style>
