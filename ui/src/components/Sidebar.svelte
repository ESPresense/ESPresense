<script>
  import SidebarItem from "./SidebarItem.svelte";
</script>

<div class="relative bg-white dark:bg-gray-800">
  <div class="flex flex-col sm:flex-row sm:justify-around">
    <div class="w-72 h-screen">
      <div class="flex items-center justify-start mx-6 mt-10">
        <svg class="h-12" viewBox="0 0 210 210" xmlns="http://www.w3.org/2000/svg">
          <circle style="fill:#fff" cx="105" cy="105" r="105"/>
          <circle cx="134.5" cy="129.5" r="6"/>
          <circle cx="104" cy="170" r="6"/>
          <circle cx="50.8" cy="63.5" r="6"/>
          <path d="m176.7 38.5-18.6 18.3a71.6 71.6 0 0 1-42 117.8 13 13 0 0 1-24.3 0A71.6 71.6 0 0 1 40.4 71.4a13 13 0 0 1 10.4-21 12.9 12.9 0 0 1 4.9 1 71.6 71.6 0 0 1 96-1L170 31.7a97.8 97.8 0 1 0 6.8 6.8z"/>
          <path d="M116.3 165.3a62.5 62.5 0 0 0 35.2-102l-13.4 13.1a43.8 43.8 0 0 1 6.4 44.6 13 13 0 0 1-16.6 19.7 43.7 43.7 0 1 1 4.2-70.4l13.1-13.4a62.4 62.4 0 0 0-82.7.6 13 13 0 0 1 1.4 6 13.1 13.1 0 0 1-13 13 13.4 13.4 0 0 1-2.7-.2 62.5 62.5 0 0 0 43.5 89 13 13 0 0 1 24.6 0z"/>
          <path d="M112.2 101.9a7.9 7.9 0 0 1 .3 2.1 8.3 8.3 0 1 1-8.3-8.3 8.5 8.5 0 0 1 2.5.4l19.4-19.6a35.1 35.1 0 1 0-3.7 57.6 13 13 0 0 1 12.2-17.7 12.8 12.8 0 0 1 2.5.2 35.1 35.1 0 0 0-5-34.1z"/>
        </svg>
        <span class="text-gray-900 dark:text-gray-100 ml-2 text-3xl font-thin">ESPresense</span>
      </div>
      <nav class="mt-10 px-6 ">
        <SidebarItem icon="contact" title="Settings" href="/settings" />
        <SidebarItem icon="device" title="Devices" href="/devices" />
        <SidebarItem icon="fingerprint" title="Fingerprints" href="/fingerprints" />
      </nav>
    </div>
  </div>
</div>
