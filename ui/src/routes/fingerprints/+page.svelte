<script lang="ts">
    import DataTable, { type Column } from "$lib/components/DataTable.svelte";
    import { devices } from "$lib/stores";
    import type { Device } from "$lib/types";

    let sortBy = $state("distance");
    let filterSelections = $state<Record<string, any>>({ vis: true });

    const columns: Column[] = [
        {
            key: "vis",
            title: "Visible",
            value: (v: Device) => v.vis ?? false,
            renderValue: (v: Device) => (v.vis ? "⬤" : ""),
            sortValue: (v: Device) => (v.vis ? 1 : 0),
            sortable: true,
            filterOptions: () => [
                { name: "⬤", value: true },
                { name: "", value: false },
            ]
        },
        {
            key: "distance",
            title: "Dist",
            value: (v: Device) => v.distance === undefined ? "n/a" : `${v.distance.toLocaleString(undefined, { minimumFractionDigits: 2 })} m`,
            sortValue: (v: Device) => v.distance ?? Number.POSITIVE_INFINITY,
            sortable: true,
            defaultSort: true,
            defaultSortDirection: "asc"
        },
        {
            key: "var",
            title: "Var",
            value: (v: Device) => v.var === undefined ? "n/a" : `${v.var.toLocaleString(undefined, { minimumFractionDigits: 2 })} m`,
            sortValue: (v: Device) => v.var ?? Number.POSITIVE_INFINITY,
            sortable: true
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
            }
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
            filterValue: (v: Device) => v.name?.charAt(0)?.toLowerCase() ?? ""
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
            filterValue: (v: Device) => v.mac?.charAt(0)?.toLowerCase() ?? ""
        },
        {
            key: "rssi",
            title: "RSSI",
            value: (v: Device) => (v.rssi != null ? `${v.rssi} dBm` : ""),
            sortValue: (v: Device) => v.rssi ?? Number.NEGATIVE_INFINITY,
            sortable: true
        },
        {
            key: "rssi@1m",
            title: "RSSI@1m",
            value: (v: Device) => (v["rssi@1m"] != null ? `${v["rssi@1m"]} dBm` : ""),
            sortValue: (v: Device) => v["rssi@1m"] ?? Number.NEGATIVE_INFINITY,
            sortable: true
        },
        {
            key: "int",
            title: "int",
            value: (v: Device) => (v.int != null ? `${v.int} ms` : ""),
            sortValue: (v: Device) => v.int ?? Number.POSITIVE_INFINITY,
            sortable: true
        },
    ];

    function rowHighlight(d: Device) {
        return d.close ? "bg-yellow-100 dark:bg-yellow-900" : "";
    }

    const rowClass = "cursor-pointer hover:bg-gray-50 dark:hover:bg-gray-700";

    let tableRows = $derived($devices?.devices || []);
</script>

<div class="bg-gray-100 dark:bg-gray-800 rounded-lg shadow">
    {#if $devices != null}
        <div class="p-6">
            <h2 class="text-xl font-semibold text-gray-900 dark:text-white mb-6">Fingerprints</h2>
            <div class="overflow-x-auto">
                <DataTable
                    {columns}
                    rows={tableRows}
                    {sortBy}
                    bind:filterSelections
                    classNameTable="min-w-full divide-y divide-gray-200 dark:divide-gray-700 table-auto"
                    classNameTh="px-6 py-4 text-left text-xs font-medium text-gray-700 dark:text-gray-200 uppercase tracking-wider bg-gray-100 dark:bg-gray-700"
                    classNameTd="px-6 py-4 text-sm text-gray-900 dark:text-gray-300 whitespace-nowrap"
                    rowClass={rowClass}
                    rowClassFn={rowHighlight}
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
