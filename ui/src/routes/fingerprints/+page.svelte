<script>
    import { devices } from "../stores";
    import SvelteTable from "svelte-table";

    var filterSelections = {
        vis: true
    };
    var sortBy = "distance";
    var sortOrder = 1;
    var selectedRowIds = [];
    const columns = [
        {
            key: "vis",
            title: "Visible",
            value: (v) => v.vis ?? false,
            renderValue: (v) => v.vis ? "⬤" : "",
            sortable: true,
            filterOptions: (rows) => {
                const options = [
                    { name: "Yes", value: true },
                    { name: "No", value: false },
                ];
                return options;
            },
        },
        {
            key: "distance",
            title: "Dist",
            value: (v) => v.distance ?? 0,
            renderValue: (v) => v.distance === undefined ? "n/a" : `${v.distance?.toLocaleString(undefined, { minimumFractionDigits: 2 })} m`,
            sortable: true,
            class: "px-0 py-0 whitespace-nowrap",
        },
        {
            key: "var",
            title: "Var",
            value: (v) => v.var ?? 0,
            renderValue: (v) => v.var === undefined ? "n/a" : `${v.var?.toLocaleString(undefined, { minimumFractionDigits: 2 })} m`,
            sortable: true,
            class: "px-0 py-0 whitespace-nowrap",
        },
        {
            key: "id",
            title: "ID",
            value: (v) => v.id ?? "",
            sortable: true,
            filterOptions: (rows) => {
                const prefixes = new Set();
                rows.forEach((row) => {
                    var prefix = row.id?.substring(0, row.id.indexOf(":") + 1);
                    if (prefix?.length > 0) {
                        prefixes.add(prefix);
                    }
                });
                return Array.from(prefixes)
                    .sort()
                    .map((a) => ({ name: a, value: a }));
            },
            filterValue: (v) => v.id?.substring(0, v.id.indexOf(":") + 1),
            headerClass: "text-left px-6 py-3",
        },
        {
            key: "name",
            title: "Name",
            value: (v) => v.name ?? "",
            sortable: true,
            filterOptions: (rows) => {
                let letrs = {};
                rows.forEach((row) => {
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
            filterValue: (v) => v.name?.charAt(0).toLowerCase(),
            headerClass: "text-left px-6 py-3",
        },
        {
            key: "mac",
            title: "MAC",
            value: (v) => v.mac ?? "n/a",
            sortable: true,
            filterOptions: (rows) => {
                // use first letter of last_name to generate filter
                let letrs = {};
                rows.forEach((row) => {
                    let letr = row.mac ? row.mac.charAt(0) : "";
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
            filterValue: (v) => v.mac.charAt(0).toLowerCase(),
            headerClass: "text-left px-6 py-3",
        },
        {
            key: "rssi",
            title: "Rssi",
            value: (v) => v.rssi,
            renderValue: (v) => v.rssi ? `${v.rssi} dBm` : "",
            sortable: true,
            headerClass: "",
            class: "whitespace-nowrap",
        },
        {
            key: "rssi@1m",
            title: "Rssi@1m",
            value: (v) => v["rssi@1m"],
            renderValue: (v) => v["rssi@1m"] ? `${v["rssi@1m"]} dBm` : "",
            sortable: true,
            headerClass: "",
            class: "whitespace-nowrap",
        },
        {
            key: "int",
            title: "int",
            value: (v) => v.int ?? 0,
            renderValue: (v) => v.int ? `${v.int} ms` : "",
            sortable: true,
            headerClass: "",
            class: "whitespace-nowrap",
        },
    ];
    function classNameRow(event) {
        event.close ? "bg-yellow-100 row" : "row";
    }
</script>

<main>
    {#if $devices?.devices != null}
        <SvelteTable {columns} rows={$devices.devices} rowKey="mac" bind:filterSelections bind:sortBy bind:sortOrder selectSingle={true} selectOnClick={true} selected={selectedRowIds} classNameTable="min-w-full divide-y divide-gray-200 table-auto" classNameThead="whitespace-nowrap text-left text-xs font-medium text-gray-500 uppercase" classNameTbody="bg-white divide-y divide-gray-200" {classNameRow} classNameRowSelected="bg-blue-100" classNameCell="px-1 py-1 whitespace-no-wrap text-sm leading-5 font-light text-gray-900" classNameInput="px-1 py-1 border rounded-md text-sm leading-5 font-medium text-gray-900 placeholder-gray-500 focus:outline-none focus:shadow-outline-blue focus:border-blue-300 focus:z-10 sm:text-sm sm:leading-5" classNameSelect="px-1 py-1 border rounded-md text-sm leading-5 font-medium text-gray-900 placeholder-gray-500 focus:outline-none focus:shadow-outline-blue focus:border-blue-300 focus:z-10 sm:text-sm sm:leading-5" />
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
