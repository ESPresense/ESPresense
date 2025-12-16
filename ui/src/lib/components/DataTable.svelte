<script lang="ts">
    export interface Column {
        key: string;
        title: string;
        value?: (row: any) => any;
        sortValue?: (row: any) => any;
        renderValue?: (row: any) => any;
        sortable?: boolean;
        defaultSort?: boolean;
        defaultSortDirection?: "asc" | "desc";
        renderComponent?: {
            component: any;
        };
        filterOptions?: (rows: any[]) => Array<{ name: string; value: string | boolean }>;
        filterValue?: (row: any) => string | boolean | undefined;
    }

    interface Props {
        columns: Column[];
        rows: any[];
        classNameTable?: string;
        classNameTh?: string;
        classNameTd?: string;
        rowClass?: string;
        rowClassFn?: (row: any) => string;
        sortBy?: string;
        filterSelections?: Record<string, any>;
        onclickRow?: (event: { row: any }) => void;
    }

    let {
        columns,
        rows,
        classNameTable = "",
        classNameTh = "",
        classNameTd = "",
        rowClass = "cursor-pointer",
        rowClassFn = undefined,
        sortBy = "",
        filterSelections = $bindable({} as Record<string, any>),
        onclickRow
    }: Props = $props();

    let sortColumn = $state("");
    let sortDirection = $state<"asc" | "desc">("asc");
    let sortInitialized = $state(false);
    let lastPropSortBy = $state("");

    $effect(() => {
        const currentSortBy = sortBy;
        const columnKeys = columns.map((c) => c.key);
        const hasCurrentSort = sortColumn && columnKeys.includes(sortColumn);

        if (currentSortBy && currentSortBy !== lastPropSortBy) {
            const column = columns.find((c) => c.key === currentSortBy);
            sortColumn = column?.key || "";
            sortDirection = column?.defaultSortDirection || "asc";
            lastPropSortBy = currentSortBy;
            sortInitialized = true;
            return;
        }

        if (!sortInitialized || !hasCurrentSort) {
            const defaultColumn = columns.find((c) => c.defaultSort);
            sortColumn = currentSortBy || defaultColumn?.key || "";
            sortDirection = defaultColumn?.defaultSortDirection || "asc";
            sortInitialized = true;
        }
    });

    function handleSort(column: Column) {
        if (!column.sortable) return;
        if (sortColumn === column.key) {
            sortDirection = sortDirection === "asc" ? "desc" : "asc";
        } else {
            sortColumn = column.key;
            sortDirection = "asc";
        }
    }

    function getFilteredRows() {
        const hasFilters = Object.keys(filterSelections || {}).length > 0;
        if (!hasFilters) return rows;

        return rows.filter((row) => {
            for (const column of columns) {
                if (!column.filterOptions) continue;
                const sel = filterSelections[column.key];
                if (sel === undefined || sel === "" || sel === null) continue;
                const val = column.filterValue
                    ? column.filterValue(row)
                    : column.value
                        ? column.value(row)
                        : row[column.key];
                if (val !== sel) return false;
            }
            return true;
        });
    }

    function getSortedRows(filtered: any[]) {
        if (!sortColumn) return filtered;
        const column = columns.find((c) => c.key === sortColumn);
        if (!column) return filtered;

        return [...filtered].sort((a, b) => {
            let aVal = column.sortValue ? column.sortValue(a) : column.value ? column.value(a) : a[column.key];
            let bVal = column.sortValue ? column.sortValue(b) : column.value ? column.value(b) : b[column.key];

            const aNullish = aVal === null || aVal === undefined;
            const bNullish = bVal === null || bVal === undefined;

            if (aNullish && bNullish) return 0;
            if (aNullish) return 1;
            if (bNullish) return -1;

            if (typeof aVal === "string" && typeof bVal === "string") {
                aVal = aVal.toLowerCase();
                bVal = bVal.toLowerCase();
            }

            let result = 0;
            if (aVal < bVal) result = -1;
            else if (aVal > bVal) result = 1;

            return sortDirection === "desc" ? -result : result;
        });
    }

    function clearFilters() {
        filterSelections = {};
    }

    function isInteractiveTarget(target: EventTarget | null) {
        if (!(target instanceof Element)) return false;
        return target.closest('button, a[href], input, select, textarea, [role="button"], [data-prevent-row-click]') !== null;
    }

    function handleRowClick(event: MouseEvent, row: any) {
        if (event.defaultPrevented) return;
        if (isInteractiveTarget(event.target)) return;
        onclickRow?.({ row });
    }

    function getCellValue(row: any, column: Column) {
        if (column.renderComponent) return undefined;
        if (column.renderValue) return column.renderValue(row);
        return column.value ? column.value(row) : row[column.key];
    }

    let filteredRows = $derived(getFilteredRows());
    let sortedRows = $derived(getSortedRows(filteredRows));
    let filterOptions = $derived.by(() => {
        const opts: Record<string, Array<{ name: string; value: string | boolean }>> = {};
        columns.forEach((c) => {
            if (c.filterOptions) {
                opts[c.key] = c.filterOptions(rows);
            }
        });
        return opts;
    });
    let hasFilters: boolean = $derived(Object.keys(filterOptions).length > 0);
</script>

<table class={classNameTable}>
    <thead>
        <tr>
            {#each columns as column}
                <th class:cursor-pointer={column.sortable} class={classNameTh}>
                    {#if column.sortable}
                        <button
                            type="button"
                            class="flex items-center gap-1 w-full text-left"
                            onclick={() => handleSort(column)}
                        >
                            {column.title}
                            {#if sortColumn === column.key}
                                <span>{sortDirection === "asc" ? "↑" : "↓"}</span>
                            {/if}
                        </button>
                    {:else}
                        {column.title}
                    {/if}
                </th>
            {/each}
        </tr>
        {#if hasFilters}
            <tr>
                {#each columns as column}
                    <th class={classNameTh}>
                        {#if filterOptions[column.key]}
                            <select
                                bind:value={filterSelections[column.key]}
                                class="w-full border border-gray-300 dark:border-gray-600 rounded px-2 py-1 bg-white dark:bg-gray-800 text-xs"
                                data-prevent-row-click
                            >
                                <option value="">All</option>
                                {#each filterOptions[column.key] as opt}
                                    <option value={opt.value}>{opt.name}</option>
                                {/each}
                            </select>
                        {/if}
                    </th>
                {/each}
            </tr>
        {/if}
    </thead>
    <tbody>
        {#each sortedRows as row}
            <tr class={`${rowClass} ${rowClassFn ? rowClassFn(row) : ""}`} onclick={(event) => handleRowClick(event, row)}>
                {#each columns as column}
                    <td class={classNameTd}>
                        {#if column.renderComponent}
                            {@const Component = column.renderComponent.component}
                            <Component {row} />
                        {:else}
                            {getCellValue(row, column) ?? ""}
                        {/if}
                    </td>
                {/each}
            </tr>
        {/each}
    </tbody>
</table>
