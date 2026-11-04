<script lang="ts">
  interface IconDef {
    w: number;
    h: number;
    name: string;
    svg: string;
  }

  const props = $props();
  const name = props.name as string;
  const width = (props.width as string) ?? "1.5rem";
  const height = (props.height as string) ?? "1.5rem";
  const focusable = (props.focusable as boolean) ?? false;
  const class_ = (props.class_ as string) ?? "";

  const icons: IconDef[] = [
    { name: "scan", w: 24, h: 24, svg: `<g fill="none" stroke="currentColor" stroke-width="1.8" stroke-linecap="round"><circle cx="12" cy="12" r="8.5"/><path d="M12 12 17 7M12 3.5v2M20.5 12h-2M12 20.5v-2M3.5 12h2"/></g>` },
    {
      w: 32,
      h: 32,
      name: "trash",
      svg: `<path d="M12 12h2v12h-2z" /><path d="M18 12h2v12h-2z" /><path d="M4 6v2h2v20a2 2 0 0 0 2 2h16a2 2 0 0 0 2-2V8h2V6zm4 22V8h16v20z" /><path d="M12 2h8v2h-8z" />`
    },
    { name: "device", w: 24, h: 24, svg: `<g fill="none" stroke="currentColor" stroke-width="1.8" stroke-linecap="round" stroke-linejoin="round"><rect x="7" y="2.5" width="10" height="19" rx="2"/><path d="M10 5h4M11 18.5h2"/></g>` },
    { name: "fingerprint", w: 24, h: 24, svg: `<g fill="none" stroke="currentColor" stroke-width="1.7" stroke-linecap="round"><path d="M5 12a7 7 0 0 1 14 0v2c0 3-1 5-2 7M8 12a4 4 0 0 1 8 0v3c0 2-.4 3.5-1 5M12 12v3c0 2-.2 4-.8 6M4 16c.2 2 0 3.8-.8 5"/></g>` },
    {
      name: "sun",
      w: 24,
      h: 24,
      svg: `<path d="M12 17a5 5 0 1 0 0-10 5 5 0 0 0 0 10zm0 2a7 7 0 1 1 0-14 7 7 0 0 1 0 14zM11 1h2v3h-2V1zm0 19h2v3h-2v-3zM3.515 4.929l1.414-1.414L7.05 5.636 5.636 7.05 3.515 4.93zM16.95 18.364l1.414-1.414 2.121 2.121-1.414 1.414-2.121-2.121zm2.121-14.85l1.414 1.415-2.121 2.121-1.414-1.414 2.121-2.121zM5.636 16.95l1.414 1.414-2.121 2.121-1.414-1.414 2.121-2.121zM23 11v2h-3v-2h3zM4 11v2H1v-2h3z"/>`
    },
    {
      name: "moon",
      w: 24,
      h: 24,
      svg: `<path d="M12 3a9 9 0 1 0 9 9c0-.46-.04-.92-.1-1.36a5.389 5.389 0 0 1-4.4 2.26 5.403 5.403 0 0 1-3.14-9.8c-.44-.06-.9-.1-1.36-.1z"/>`
    },
    {
      name: "system",
      w: 24,
      h: 24,
      svg: `<path d="M3 5a2 2 0 0 1 2-2h14a2 2 0 0 1 2 2v9a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2V5zm2 0v9h14V5H5zm4 14h6v2H9v-2z"/>`
    },
    { name: "network", w: 24, h: 24, svg: `<g fill="none" stroke="currentColor" stroke-width="1.8" stroke-linecap="round"><path d="M2 8.8a15 15 0 0 1 20 0M5 12a10.5 10.5 0 0 1 14 0m-11 3.2a6 6 0 0 1 8 0"/><circle cx="12" cy="19" r=".8" fill="currentColor" stroke="none"/></g>` },
    { name: "hardware", w: 24, h: 24, svg: `<g fill="none" stroke="currentColor" stroke-width="1.8" stroke-linecap="round" stroke-linejoin="round"><rect x="7" y="7" width="10" height="10" rx="2"/><path d="M10 10h4v4h-4zM9 2v5m6-5v5m-6 10v5m6-5v5M2 9h5m-5 6h5m10-6h5m-5 6h5"/></g>` },
    { name: "template", w: 24, h: 24, svg: `<g fill="none" stroke="currentColor" stroke-width="1.8" stroke-linecap="round" stroke-linejoin="round"><path d="M6 3h8l4 4v14H6z"/><path d="M14 3v5h5M9 12h6m-6 4h6"/></g>` },
    {
      name: "logo",
      w: 210,
      h: 210,
      svg: `<circle class="fill-gray-50 dark:fill-gray-800" cx="105" cy="105" r="105" />
            <circle class="fill-current text-gray-800 dark:text-gray-100" cx="134.5" cy="129.5" r="6" />
            <circle class="fill-current text-gray-800 dark:text-gray-100" cx="104" cy="170" r="6" />
            <circle class="fill-current text-gray-800 dark:text-gray-100" cx="50.8" cy="63.5" r="6" />
            <path class="fill-current text-gray-800 dark:text-gray-100" d="m176.7 38.5-18.6 18.3a71.6 71.6 0 0 1-42 117.8 13 13 0 0 1-24.3 0A71.6 71.6 0 0 1 40.4 71.4a13 13 0 0 1 10.4-21 12.9 12.9 0 0 1 4.9 1 71.6 71.6 0 0 1 96-1L170 31.7a97.8 97.8 0 1 0 6.8 6.8z" />
            <path class="fill-current text-gray-800 dark:text-gray-100" d="M116.3 165.3a62.5 62.5 0 0 0 35.2-102l-13.4 13.1a43.8 43.8 0 0 1 6.4 44.6 13 13 0 0 1-16.6 19.7 43.7 43.7 0 1 1 4.2-70.4l13.1-13.4a62.4 62.4 0 0 0-82.7.6 13 13 0 0 1 1.4 6 13.1 13.1 0 0 1-13 13 13.4 13.4 0 0 1-2.7-.2 62.5 62.5 0 0 0 43.5 89 13 13 0 0 1 24.6 0z" />
            <path class="fill-current text-gray-800 dark:text-gray-100" d="M112.2 101.9a7.9 7.9 0 0 1 .3 2.1 8.3 8.3 0 1 1-8.3-8.3 8.5 8.5 0 0 1 2.5.4l19.4-19.6a35.1 35.1 0 1 0-3.7 57.6 13 13 0 0 1 12.2-17.7 12.8 12.8 0 0 1 2.5.2 35.1 35.1 0 0 0-5-34.1z" />`
    }
  ];

  const displayIcon = $derived(icons.find((e) => e.name === name));
</script>

{#if displayIcon}
  <svg
    fill="currentColor"
    class={`inline-block ${class_}`}
    focusable={focusable ? 'true' : 'false'}
    {width}
    {height}
    viewBox="0 0 {displayIcon.w} {displayIcon.h}"
  >
    {@html displayIcon.svg}
  </svg>
{/if}
