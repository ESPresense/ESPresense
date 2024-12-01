<script lang="ts">
  import Icon from "./Icon.svelte";
  import { page } from "$app/stores";

  const props = $props();
  const title = props.title as string ?? "Title";
  const icon = props.icon as string ?? "";
  const href = props.href as string ?? "/";
  const count = props.count as number ?? 0;

  const isActive = $derived($page.url.pathname === href);
</script>

<a {href} data-sveltekit-preload-data
  class={`flex items-center p-3 my-2 transition-colors duration-200 rounded-lg
    hover:bg-gray-200 hover:text-gray-900 dark:hover:bg-gray-700 dark:hover:text-white
    ${isActive
      ? "bg-gray-200 text-gray-900 dark:bg-gray-700 dark:text-white"
      : "text-gray-700 dark:text-gray-300"}`}>
  <Icon name={icon} class_="w-6 h-6" />
  <span class="mx-4 text-base font-medium">
    {title}
  </span>

  {#if count > 0}
    <span class="flex-grow text-right">
      <button type="button" class="w-6 h-6 text-xs rounded-full text-white bg-red-500">
        <span class="p-1">
          {count}
        </span>
      </button>
    </span>
  {:else}
    <span class="flex-grow text-right"></span>
  {/if}
</a>
