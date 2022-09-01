<script>
  import { devices, events } from '../stores';
  import SvelteTable from "svelte-table";

var filterSelections = {  };
var sortBy = "distance"
var sortOrder = 1;
const columns = [
  {
    key: "distance",
    title: "Dist",
    value: v => v.distance,
    renderValue: v => `${v.distance.toLocaleString(undefined, { minimumFractionDigits: 2 })}m`,
    sortable: true,
    class:"px-0 py-0 whitespace-nowrap"
  },
  {
    key: "id",
    title: "ID",
    value: v => v.id,
    sortable: true,
    filterOptions: rows => {
      const prefixes = new Set()
      rows.forEach(row => {
        var prefix = row.id.substring(0, row.id.indexOf(":")+1);
        if (prefix.length > 0) {
          prefixes.add(prefix);
        }
      });
      return Array.from(prefixes).sort().map(a=>({"name": a, "value": a}));
    },
    filterValue: v => v.id.substring(0, v.id.indexOf(":")+1),
    headerClass: "text-left px-6 py-3",
  },
  {
    key: "name",
    title: "Name",
    value: v => v.name ?? "",
    sortable: true,
    filterOptions: rows => {
      let letrs = {};
      rows.forEach(row => {
        let letr = row.name?.charAt(0);
        if (letr && letrs[letr] === undefined)
          letrs[letr] = {
            name: `${letr.toUpperCase()}`,
            value: letr.toLowerCase(),
          };
      });
      // fix order
      letrs = Object.entries(letrs)
        .sort()
        .reduce((o, [k, v]) => ((o[k] = v), o), {});
      return Object.values(letrs);
    },
    filterValue: v => v.name?.charAt(0).toLowerCase(),
    headerClass: "text-left px-6 py-3",
  },
  {
    key: "mac",
    title: "MAC",
    value: v => v.mac,
    sortable: true,
    filterOptions: rows => {
      // use first letter of last_name to generate filter
      let letrs = {};
      rows.forEach(row => {
        let letr = row.mac.charAt(0);
        if (letrs[letr] === undefined)
          letrs[letr] = {
            name: `${letr.toUpperCase()}`,
            value: letr.toLowerCase(),
          };
      });
      // fix order
      letrs = Object.entries(letrs)
        .sort()
        .reduce((o, [k, v]) => ((o[k] = v), o), {});
      return Object.values(letrs);
    },
    filterValue: v => v.mac.charAt(0).toLowerCase(),
    headerClass: "text-left px-6 py-3",
  },
  {
    key: "rssi",
    title: "Rssi",
    value: v => v.rssi,
    renderValue: v => v.rssi + "dBm",
    sortable: true,
    headerClass: "text-left px-6 py-3",
  },
    {
    key: "rssi@1m",
    title: "Rssi@1m",
    value: v => v["rssi@1m"],
    renderValue: v => v["rssi@1m"] + "dBm",
    sortable: true,
    headerClass: "text-left px-6 py-3",
  },
  {
    key: "interval",
    title: "Interval",
    value: v => v.interval,
    renderValue: v => v.interval + "ms",
    sortable: true,
    headerClass: "text-left px-6 py-3",
  },
];
</script>

<main>

{#if $devices?.devices != null }
<SvelteTable columns="{columns}" rows="{$devices.devices}"
  bind:filterSelections="{filterSelections}"
  bind:sortBy = "{sortBy}"
  bind:sortOrder = "{sortOrder}"
  classNameTable="min-w-full divide-y divide-gray-200 table-auto"
  classNameThead="whitespace-nowrap text-left text-xs font-medium text-gray-500 uppercase"
  classNameTbody="bg-white divide-y divide-gray-200"
  classNameCell="px-3 py-1 whitespace-no-wrap text-sm leading-5 font-light text-gray-900"
  classNameInput="px-1 py-1 border rounded-md text-sm leading-5 font-medium text-gray-900 placeholder-gray-500 focus:outline-none focus:shadow-outline-blue focus:border-blue-300 focus:z-10 sm:text-sm sm:leading-5"
  classNameSelect="px-1 py-1 border rounded-md text-sm leading-5 font-medium text-gray-900 placeholder-gray-500 focus:outline-none focus:shadow-outline-blue focus:border-blue-300 focus:z-10 sm:text-sm sm:leading-5"
></SvelteTable>
{:else}
    <h1>Loading fingerprints...</h1>
{/if}
</main>

<style>
  main {
    padding: 1rem;
  }
  h1 {
    text-align: center;
  }
</style>
