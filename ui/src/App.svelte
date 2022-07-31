<script>
  import { devices } from './stores';
  import SvelteTable from "svelte-table";

const columns = [
  {
    key: "distance",
    title: "Distance",
    value: v => v.distance,
    renderValue: v => `${v.distance.toLocaleString(undefined, { minimumFractionDigits: 2 })}m`,
    sortable: true,
  },
  {
    key: "id",
    title: "ID",
    value: v => v.id,
    sortable: true,
    filterOptions: rows => {
      // generate groupings of 0-10, 10-20 etc...
      let nums = {};
      rows.forEach(row => {
        let num = Math.floor(row.id / 10);
        if (nums[num] === undefined)
          nums[num] = { name: `${num * 10} to ${(num + 1) * 10}`, value: num };
      });
      // fix order
      nums = Object.entries(nums)
        .sort()
        .reduce((o, [k, v]) => ((o[k] = v), o), {});
      return Object.values(nums);
    },
    filterValue: v => Math.floor(v.id / 10),
    headerClass: "text-left",
  },
  {
    key: "name",
    title: "Name",
    value: v => v.name ?? "",
    sortable: true,
    filterOptions: rows => {
      // use first letter of first_name to generate filter
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
    filterValue: v => v.first_name.charAt(0).toLowerCase(),
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
    filterValue: v => v.last_name.charAt(0).toLowerCase(),
  },
  {
    key: "rssi",
    title: "Rssi",
    value: v => v.rssi,
    renderValue: v => v.rssi+"dBm",
    sortable: true,
  },
    {
    key: "rssi@1m",
    title: "Rssi@1m",
    value: v => v["rssi@1m"],
    renderValue: v => v["rssi@1m"]+"dBm",
    sortable: true,
  },
  {
    key: "interval",
    title: "Interval",
    value: v => v.interval,
    renderValue: v => v.interval + "ms",
    sortable: true,
  },
];
</script>

<main>

{#if $devices != null}
  <SvelteTable columns="{columns}" rows="{$devices}"></SvelteTable>
{:else}
    <h1>Error while loading devices</h1>
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
