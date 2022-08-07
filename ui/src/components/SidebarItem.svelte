<script>
  import Icon from "./Icon.svelte";
  import active from "svelte-spa-router/active";
  import { link } from "svelte-spa-router";

  export let title = "Title";
  export let icon = "";
  export let href = "/";
  export let count = 0;
</script>

<a use:link use:active={{ className: "text-gray-800 dark:text-gray-100 rounded-lg bg-gray-100 dark:bg-gray-600", inactiveClassName: "text-gray-600 dark:text-gray-400 rounded-lg" }} class="hover:text-gray-800 hover:bg-gray-100 flex items-center p-2 my-6 transition-colors dark:hover:text-white dark:hover:bg-gray-600 duration-200 text-gray-600 dark:text-gray-400 rounded-lg" {href}>
  <Icon name={icon} />
  <span class="mx-4 text-lg font-normal">
    {title}
  </span>

  {#if count > 0}
    <span class="flex-grow text-right">
      <button type="button" class="w-6 h-6 text-xs  rounded-full text-white bg-red-500">
        <span class="p-1">
          {count}
        </span>
      </button>
    </span>
  {:else}
    <span class="flex-grow text-right" />
  {/if}
</a>
