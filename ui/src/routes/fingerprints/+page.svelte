<script lang="ts">
    import SvelteTable from "svelte-table";
    import { devices } from "$lib/stores";
    import type { Device, TableColumn } from "$lib/types";

    let filterSelections = $state({
        vis: true
    });
    let sortBy = $state("distance");
    let sortOrder = $state<-1 | 0 | 1>(1);
    let selectedRowIds = $state([]);

    const columns: TableColumn<Device>[] = [
        {
            key: "vis",
            title: "Visible",
            value: (v: Device) => v.vis ?? false,
            renderValue: (v: Device) => v.vis ?? false ? "⬤" : "",
            sortable: true,
            filterOptions: () => [
                { name: "Yes", value: true },
                { name: "No", value: false },
            ],
            headerClass: "px-6 py-4 text-left text-xs font-medium text-gray-700 dark:text-gray-200 uppercase tracking-wider",
            class: "text-center",
        },
        {
            key: "distance",
            title: "Dist",
            value: (v: Device) => v.distance ?? 0,
            renderValue: (v: Device) => v.distance === undefined ? "n/a" : `${v.distance.toLocaleString(undefined, { minimumFractionDigits: 2 })} m`,
            sortable: true,
            headerClass: "px-6 py-4 text-left text-xs font-medium text-gray-700 dark:text-gray-200 uppercase tracking-wider",
            class: "px-6 py-4 whitespace-nowrap",
        },
        {
            key: "var",
            title: "Var",
            value: (v: Device) => v.var ?? 0,
            renderValue: (v: Device) => v.var === undefined ? "n/a" : `${v.var.toLocaleString(undefined, { minimumFractionDigits: 2 })} m`,
            sortable: true,
            headerClass: "px-6 py-4 text-left text-xs font-medium text-gray-700 dark:text-gray-200 uppercase tracking-wider",
            class: "px-6 py-4 whitespace-nowrap",
        },
        {
            key: "id",
            title: "ID",
            value: (v: Device) => v.id ?? "",
            sortable: true,
            filterOptions: (rows: Device[]) => {
                const prefixes = new Set<string>();
                rows.forEach((row) => {
                    if (row.id) {
                        const colonIndex = row.id.indexOf(":");
                        if (colonIndex !== -1) {
                            const prefix = row.id.substring(0, colonIndex + 1);
                            prefixes.add(prefix);
                        }
                    }
                });
                return Array.from(prefixes)
                    .sort()
                    .map((a) => ({ name: a, value: a }));
            },
            filterValue: (v: Device) => {
                if (!v.id) return "";
                const colonIndex = v.id.indexOf(":");
                return colonIndex !== -1 ? v.id.substring(0, colonIndex + 1) : "";
            },
            headerClass: "px-6 py-4 text-left text-xs font-medium text-gray-700 dark:text-gray-200 uppercase tracking-wider",
            class: "px-6 py-4",
        },
        {
            key: "name",
            title: "Name",
            value: (v: Device) => v.name ?? "",
            sortable: true,
            filterOptions: (rows: Device[]) => {
                const letrs: Record<string, { name: string; value: string }> = {};
                rows.forEach((row) => {
                    const letr = row.name?.charAt(0);
                    if (letr && !letrs[letr]) {
                        letrs[letr] = {
                            name: letr.toUpperCase(),
                            value: letr.toLowerCase(),
                        };
                    }
                });
                return Object.values(letrs).sort((a, b) => a.name.localeCompare(b.name));
            },
            filterValue: (v: Device) => v.name?.charAt(0)?.toLowerCase() ?? "",
            headerClass: "px-6 py-4 text-left text-xs font-medium text-gray-700 dark:text-gray-200 uppercase tracking-wider",
            class: "px-6 py-4",
        },
        {
            key: "mac",
            title: "MAC",
            value: (v: Device) => v.mac ?? "n/a",
            sortable: true,
            filterOptions: (rows: Device[]) => {
                const letrs: Record<string, { name: string; value: string }> = {};
                rows.forEach((row) => {
                    const letr = row.mac?.charAt(0);
                    if (letr && !letrs[letr]) {
                        letrs[letr] = {
                            name: letr.toUpperCase(),
                            value: letr.toLowerCase(),
                        };
                    }
                });
                return Object.values(letrs).sort((a, b) => a.name.localeCompare(b.name));
            },
            filterValue: (v: Device) => v.mac?.charAt(0)?.toLowerCase() ?? "",
            headerClass: "px-6 py-4 text-left text-xs font-medium text-gray-700 dark:text-gray-200 uppercase tracking-wider",
            class: "px-6 py-4 font-mono",
        },
        {
            key: "rssi",
            title: "Rssi",
            value: (v: Device) => v.rssi ?? 0,
            renderValue: (v: Device) => v.rssi ? `${v.rssi} dBm` : "",
            sortable: true,
            headerClass: "px-6 py-4 text-left text-xs font-medium text-gray-700 dark:text-gray-200 uppercase tracking-wider",
            class: "px-6 py-4 whitespace-nowrap",
        },
        {
            key: "rssi@1m",
            title: "Rssi@1m",
            value: (v: Device) => v["rssi@1m"] ?? 0,
            renderValue: (v: Device) => v["rssi@1m"] ? `${v["rssi@1m"]} dBm` : "",
            sortable: true,
            headerClass: "px-6 py-4 text-left text-xs font-medium text-gray-700 dark:text-gray-200 uppercase tracking-wider",
            class: "px-6 py-4 whitespace-nowrap",
        },
        {
            key: "int",
            title: "int",
            value: (v: Device) => v.int ?? 0,
            renderValue: (v: Device) => v.int ? `${v.int} ms` : "",
            sortable: true,
            headerClass: "px-6 py-4 text-left text-xs font-medium text-gray-700 dark:text-gray-200 uppercase tracking-wider",
            class: "px-6 py-4 whitespace-nowrap",
        },
    ];

    function classNameRow(d: Device) {
        return d.close ? "bg-yellow-100 dark:bg-yellow-900" : "";
    }

    let tableRows = $derived($devices?.devices || []);
</script>

<div class="bg-gray-100 dark:bg-gray-800 rounded-lg shadow-sm">
    {#if $devices != null}
        <div class="p-6">
            <h2 class="text-xl font-semibold text-gray-900 dark:text-white mb-6">Fingerprints</h2>
            <div class="overflow-x-auto">
                <SvelteTable
                    {columns}
                    rows={tableRows}
                    rowKey="mac"
                    bind:filterSelections
                    bind:sortBy
                    bind:sortOrder
                    selectSingle={true}
                    selectOnClick={true}
                    bind:selected={selectedRowIds}
                    classNameTable="min-w-full divide-y divide-gray-200 dark:divide-gray-700 table-auto"
                    classNameThead="bg-gray-100 dark:bg-gray-700"
                    classNameTbody="bg-white dark:bg-gray-800 divide-y divide-gray-200 dark:divide-gray-700"
                    {classNameRow}
                    classNameRowSelected="bg-blue-50 dark:bg-blue-900"
                    classNameCell="text-sm text-gray-900 dark:text-gray-300"
                    classNameInput="mt-1 block w-full border-gray-300 dark:border-gray-600 dark:bg-gray-700 dark:text-white shadow-sm focus:border-blue-500 focus:ring-blue-500"
                    classNameSelect="mt-1 block w-full border-gray-300 dark:border-gray-600 dark:bg-gray-700 dark:text-white shadow-sm focus:border-blue-500 focus:ring-blue-500"
                />
            </div>
        </div>
    {:else}
        <div class="flex items-center justify-center min-h-[50vh]">
            <div class="text-center">
                <h1 class="text-xl font-medium text-gray-700 dark:text-gray-300">Loading fingerprints...</h1>
                <p class="mt-2 text-sm text-gray-600 dark:text-gray-400">Please wait while we fetch the data.</p>
            </div>
        </div>
    {/if}
</div>
