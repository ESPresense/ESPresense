<script lang="ts">
    import DataTable, { type Column } from "#lib/components/DataTable.svelte";
    import { Dialog } from "@skeletonlabs/skeleton-svelte";
    import { configs, events, enroll, cancelEnroll, aliasDevice, findConfig } from "#lib/stores.js";
    import type { Config, Events, Devices, Device } from "#lib/types.js";
    import { deviceTypes, suggestAlias } from "#lib/utils/alias.js";

    let name = $state("");
    let id = $state("");
    let deviceType = $state("");
    let showModal = $state(false);
    let showEditModal = $state(false);
    let showAliasModal = $state(false);
    let aliasCandidates = $state<string[]>([]);
    let aliasStep = $state<'near' | 'far'>('near');
    let deviceId = $state("");
    let deviceMac = $state<string | undefined>(undefined);
    let aliasSaving = $state(false);
    let aliasError = $state("");
    // MAC of each id seen close during Find, to match a config keyed by MAC at save time
    let seenMacs: Record<string, string | undefined> = {};
    let sortBy = $state("alias");
    let filterSelections = $state<Record<string, any>>({});
    let editingConfig = $state<Config | null>(null);

    function onSelectExisting(event: Event) {
        const alias = (event.target as HTMLSelectElement).value;
        if (!alias) {
            id = "";
            name = "";
            return;
        }
        const cfg = tableRows.find((c) => c.alias === alias);
        if (cfg) {
            id = cfg.alias || "";
            name = cfg.name || "";
        }
    }

    let modalTitle = $derived(($events as Events)?.room ? `Enroll Device - ${($events as Events).room}` : "Enroll Device");
    let tableRows = $derived($configs?.configs || []);

    function resetEnrollState() {
        name = "";
        id = "";
        deviceType = "";
        deviceId = "";
        deviceMac = undefined;
        aliasError = "";
    }

    function handleEnrollOpenChange(event: { open: boolean }) {
        showModal = event.open;
        if (!event.open) {
            // Only cancel if we're currently in the enrolling state
            if (($events as Events)?.state?.enrolling) {
                cancelEnroll();
            }
            resetEnrollState();
        }
    }

    async function getCloseDeviceIds(): Promise<string[]> {
        try {
            const response = await fetch('/json/devices?showAll');
            const data: Devices = await response.json();
            const close = data.devices.filter((d: Device) => d.close && (d.id || d.mac));
            for (const d of close) seenMacs[(d.id || d.mac)!] = d.mac;
            return close.map((d: Device) => (d.id || d.mac)!);
        } catch {
            return [];
        }
    }

    async function detect() {
        const closeIds = await getCloseDeviceIds();
        if (aliasStep === 'near') {
            aliasCandidates = aliasCandidates.length === 0 ? closeIds : aliasCandidates.filter((id) => closeIds.includes(id));
            if (aliasCandidates.length > 1) {
                aliasStep = 'far';
            }
        } else {
            aliasCandidates = aliasCandidates.filter((id) => !closeIds.includes(id));
            if (aliasCandidates.length > 1) {
                aliasStep = 'near';
            }
        }
    }

    function openAlias() {
        showAliasModal = true;
        aliasCandidates = [];
        aliasStep = 'near';
        deviceId = "";
        deviceMac = undefined;
        seenMacs = {};
    }

    function closeAlias() {
        showAliasModal = false;
        aliasCandidates = [];
        aliasStep = 'near';
        deviceId = "";
        deviceMac = undefined;
        seenMacs = {};
    }

    function useId() {
        const selected = aliasCandidates[0];
        const mac = seenMacs[selected];
        showAliasModal = false;
        aliasCandidates = [];
        aliasStep = 'near';
        seenMacs = {};
        deviceId = selected;
        deviceMac = mac;
        // An already configured device starts from its config, so saving keeps its name and alias.
        const cfg = findConfig(selected, mac, $configs?.configs);
        id = cfg?.alias || selected;
        name = cfg?.name ?? "";
        showModal = true;
    }

    async function onEnroll() {
        const alias = id || suggestAlias(name, deviceType);
        if (!deviceId) {
            enroll(alias, name);
            return;
        }
        if (aliasSaving) return;
        aliasSaving = true;
        aliasError = "";
        try {
            await aliasDevice(deviceId, deviceMac, alias, name, $configs?.configs);
            showModal = false;
            resetEnrollState();
        } catch {
            // Keep the dialog and the form so the user can retry.
            if (showModal) aliasError = "The node didn't save it. Try again.";
        } finally {
            aliasSaving = false;
        }
    }

    function handleEditOpenChange(event: { open: boolean }) {
        showEditModal = event.open;
        if (!event.open) {
            editingConfig = null;
        }
    }

    async function onSaveEdit() {
        if (!editingConfig) return;

        try {
            const response = await fetch('/json/configs', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({
                    id: editingConfig.id,
                    alias: editingConfig.alias,
                    name: editingConfig.name,
                    "rssi@1m": editingConfig["rssi@1m"]
                })
            });

            if (!response.ok) {
                throw new Error('Failed to save device');
            }

            showEditModal = false;
            editingConfig = null;
        } catch (error) {
            console.error('Error saving device:', error);
            alert('Failed to save device changes');
        }
    }

    async function onDeleteConfig() {
        if (!editingConfig) return;

        if (!confirm(`Are you sure you want to delete the device "${editingConfig.name || editingConfig.alias || editingConfig.id}"?`)) {
            return;
        }

        try {
            const response = await fetch(`/json/configs?id=${encodeURIComponent(editingConfig.id ?? '')}`, {
                method: 'DELETE'
            });

            if (!response.ok) {
                throw new Error('Failed to delete device');
            }

            showEditModal = false;
            editingConfig = null;
        } catch (error) {
            console.error('Error deleting device:', error);
            alert('Failed to delete device');
        }
    }

    function onRowClick(row: Config) {
        editingConfig = { ...row };
        showEditModal = true;
    }

    function formatRemainingTime(seconds: number) {
        const minutes = Math.floor(seconds / 60);
        const remainingSeconds = seconds % 60;
        return [minutes, remainingSeconds].map((val) => val.toString().padStart(2, "0")).join(":");
    }

    const columns: Column[] = [
        {
            key: "alias",
            title: "Alias",
            value: (v: Config) => v.alias ?? "",
            sortable: true,
            filterOptions: (rows: Config[]) => {
                const prefixes = new Set<string>();
                rows.forEach((row) => {
                    if (row.alias) {
                        const colonIndex = row.alias.indexOf(":");
                        if (colonIndex !== -1) {
                            const prefix = row.alias.substring(0, colonIndex + 1);
                            prefixes.add(prefix);
                        }
                    }
                });
                return Array.from(prefixes)
                    .sort()
                    .map((a) => ({ name: a, value: a }));
            },
            filterValue: (v: Config) => {
                if (!v.alias) return "";
                const colonIndex = v.alias.indexOf(":");
                return colonIndex !== -1 ? v.alias.substring(0, colonIndex + 1) : "";
            }
        },
        {
            key: "id",
            title: "ID",
            value: (v: Config) => v.id ?? "",
            sortable: true,
            filterOptions: (rows: Config[]) => {
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
            filterValue: (v: Config) => {
                if (!v.id) return "";
                const colonIndex = v.id.indexOf(":");
                return colonIndex !== -1 ? v.id.substring(0, colonIndex + 1) : "";
            }
        },
        {
            key: "name",
            title: "Name",
            value: (v: Config) => v.name ?? "",
            sortable: true,
            filterOptions: (rows: Config[]) => {
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
            filterValue: (v: Config) => v.name?.charAt(0)?.toLowerCase() ?? ""
        },
        {
            key: "rssi@1m",
            title: "RSSI@1m",
            value: (v: Config) => (v["rssi@1m"] != null ? `${v["rssi@1m"]} dBm` : ""),
            sortValue: (v: Config) => v["rssi@1m"] ?? Number.POSITIVE_INFINITY,
            sortable: true
        },
    ];

    const rowClass = "cursor-pointer hover:bg-gray-50 dark:hover:bg-gray-700";
</script>

<div class="content-card">
    <div class="p-6">
        <div class="flex justify-between items-center mb-6">
            <h2 class="text-xl font-semibold text-gray-900 dark:text-white">Devices</h2>
            <div class="flex space-x-2">
                <button
                    onclick={openAlias}
                    class="text-white bg-blue-600 hover:bg-blue-700 focus:ring-4 focus:outline-none focus:ring-blue-300 dark:bg-blue-500 dark:hover:bg-blue-600 dark:focus:ring-blue-800"
                >
                    Alias
                </button>
                <button
                    onclick={() => (showModal = true)}
                    class="text-white bg-blue-600 hover:bg-blue-700 focus:ring-4 focus:outline-none focus:ring-blue-300 dark:bg-blue-500 dark:hover:bg-blue-600 dark:focus:ring-blue-800"
                >
                    Enroll
                </button>
            </div>
        </div>

        {#if $configs != null}
            <div class="overflow-x-auto">
                <DataTable
                    {columns}
                    rows={tableRows}
                    {sortBy}
                    bind:filterSelections
                    classNameTable="min-w-full divide-y divide-gray-200 dark:divide-gray-700 table-auto"
                    classNameTh="px-6 py-4 text-left text-xs font-semibold text-gray-500 dark:text-gray-300 uppercase tracking-wider bg-gray-50 dark:bg-gray-700/70"
                    classNameTd="px-6 py-4 text-sm text-gray-900 dark:text-gray-300 whitespace-nowrap"
                    rowClass={rowClass}
                    onclickRow={(event) => onRowClick(event.row)}
                />
            </div>
        {:else}
            <div class="flex items-center justify-center min-h-[50vh]">
                <div class="text-center">
                    <h1 class="text-xl font-medium text-gray-700 dark:text-gray-300">Loading configured devices...</h1>
                    <p class="mt-2 text-sm text-gray-600 dark:text-gray-400">Please wait while we fetch the data.</p>
                </div>
            </div>
        {/if}
    </div>
</div>

<Dialog open={showModal} onOpenChange={handleEnrollOpenChange} modal={false} preventScroll={true}>
    <Dialog.Backdrop class="fixed inset-0 bg-gray-500/75 transition-opacity" />
    <Dialog.Positioner class="fixed inset-0 flex items-center justify-center w-full h-full p-4">
        <Dialog.Content class="relative bg-white dark:bg-gray-800 rounded-lg shadow-xl max-w-lg w-full">
            <div class="flex justify-between items-start p-4 border-b dark:border-gray-700">
                <Dialog.Title class="text-xl font-semibold text-gray-900 dark:text-white">{modalTitle}</Dialog.Title>
                <Dialog.CloseTrigger
                    type="button"
                    class="text-gray-400 bg-transparent hover:bg-gray-200 hover:text-gray-900 ml-auto inline-flex items-center dark:hover:bg-gray-700 dark:hover:text-white"
                >
                    <span class="sr-only">Close</span>
                    <svg class="w-5 h-5" fill="currentColor" viewBox="0 0 20 20" xmlns="http://www.w3.org/2000/svg"><path fill-rule="evenodd" d="M4.293 4.293a1 1 0 011.414 0L10 8.586l4.293-4.293a1 1 0 111.414 1.414L11.414 10l4.293 4.293a1 1 0 01-1.414 1.414L10 11.414l-4.293 4.293a1 1 0 01-1.414-1.414L8.586 10 4.293 5.707a1 1 0 010-1.414z" clip-rule="evenodd"></path></svg>
                </Dialog.CloseTrigger>
            </div>
            <div class="p-6 space-y-6">
                {#if !($events as Events)?.state?.enrolling}
                    {#if ($events as Events)?.state?.enrolledId}
                        <p class="text-sm leading-5 text-gray-600 dark:text-gray-400">
                            The device has been enrolled. ESPresense has disconnected; you can delete the pairing on your device. The new device ID is:
                            <span class="font-semibold text-gray-900 dark:text-white">{($events as Events).state?.enrolledId}</span>.
                        </p>
                    {:else}
                        <div class="space-y-4">
                            <select
                                onchange={onSelectExisting}
                                class="mt-1 block w-full border-gray-300 dark:border-gray-600 dark:bg-gray-700 dark:text-white shadow-sm focus:border-blue-500 focus:ring-blue-500"
                            >
                                <option value="">Select existing ID</option>
                                {#each tableRows.filter(cfg => cfg.alias && cfg.alias.trim() !== '').sort((a, b) => a.alias!.localeCompare(b.alias!)) as cfg}
                                    <option value={cfg.alias}>{cfg.alias}</option>
                                {/each}
                            </select>
                            <select
                                bind:value={deviceType}
                                class="mt-1 block w-full border-gray-300 dark:border-gray-600 dark:bg-gray-700 dark:text-white shadow-sm focus:border-blue-500 focus:ring-blue-500"
                            >
                                <option value="">Select device type</option>
                                {#each deviceTypes as type}
                                    <option value={type}>{type}</option>
                                {/each}
                            </select>
                            <input
                                class="mt-1 block w-full border-gray-300 dark:border-gray-600 dark:bg-gray-700 dark:text-white shadow-sm focus:border-blue-500 focus:ring-blue-500"
                                type="text"
                                bind:value={name}
                                placeholder="Enter name"
                            />
                            <input
                                class="mt-1 block w-full border-gray-300 dark:border-gray-600 dark:bg-gray-700 dark:text-white shadow-sm focus:border-blue-500 focus:ring-blue-500"
                                type="text"
                                bind:value={id}
                                placeholder={name ? suggestAlias(name, deviceType) : "Enter custom ID or leave empty for auto-generated"}
                            />
                            <p class="text-sm text-gray-600 dark:text-gray-400">Leave empty to use auto-generated ID</p>
                            {#if aliasError}
                                <p role="alert" class="text-sm text-red-600 dark:text-red-400">{aliasError}</p>
                            {/if}
                        </div>
                    {/if}
                {:else}
                    <p class="text-sm leading-5 text-gray-600 dark:text-gray-400">To begin, navigate to your device's Bluetooth settings (on Apple Watch use the BluetoothLE app) and pair with the ESPresense device.</p>
                    <p class="mt-4 text-sm leading-5 text-gray-600 dark:text-gray-400">Start the pairing process now. Time remaining: {formatRemainingTime(($events as Events).state?.remain ?? 0)}</p>
                {/if}
            </div>
            <div class="flex items-center justify-end p-6 space-x-4 border-t border-gray-200 dark:border-gray-700">
                {#if !($events as Events)?.state?.enrolling && !($events as Events)?.state?.enrolledId}
                    <button
                        onclick={onEnroll}
                        disabled={!name || aliasSaving}
                        class="text-white bg-blue-600 hover:bg-blue-700 focus:ring-4 focus:outline-none focus:ring-blue-300 text-center dark:bg-blue-500 dark:hover:bg-blue-600 dark:focus:ring-blue-800 disabled:opacity-50 disabled:cursor-not-allowed"
                    >
                        Enroll
                    </button>
                {/if}
                <Dialog.CloseTrigger
                    type="button"
                    class="text-gray-500 bg-white hover:bg-gray-100 focus:ring-4 focus:outline-none focus:ring-blue-300 border border-gray-200 hover:text-gray-900 focus:z-10 dark:bg-gray-800 dark:text-gray-300 dark:border-gray-600 dark:hover:text-white dark:hover:bg-gray-700 dark:focus:ring-gray-700"
                >
                    Close
                </Dialog.CloseTrigger>
            </div>
        </Dialog.Content>
    </Dialog.Positioner>
</Dialog>

{#if showAliasModal}
    <Dialog open={showAliasModal} onOpenChange={(e) => { if (!e.open) closeAlias(); }} modal={false} preventScroll={true}>
        <Dialog.Backdrop class="fixed inset-0 z-50 bg-gray-900/50 transition-opacity" />
        <Dialog.Positioner class="fixed inset-0 z-50 flex items-center justify-center w-full h-full p-4">
            <Dialog.Content class="relative bg-white dark:bg-gray-800 rounded-lg shadow-xl max-w-md w-full p-6">
                <Dialog.Title class="text-lg font-semibold mb-4 text-gray-900 dark:text-white">Alias Device</Dialog.Title>
                {#if aliasCandidates.length === 1}
                    <p class="mb-4 text-gray-900 dark:text-white">Device identified: {aliasCandidates[0]}</p>
                    <div class="flex justify-end space-x-2">
                        <button
                            class="text-white bg-blue-600 hover:bg-blue-700 focus:ring-4 focus:outline-none focus:ring-blue-300 text-center dark:bg-blue-500 dark:hover:bg-blue-600 dark:focus:ring-blue-800"
                            onclick={useId}
                        >
                            Use ID
                        </button>
                        <Dialog.CloseTrigger
                            type="button"
                            class="text-gray-500 bg-white hover:bg-gray-100 focus:ring-4 focus:outline-none focus:ring-blue-300 border border-gray-200 hover:text-gray-900 focus:z-10 dark:bg-gray-800 dark:text-gray-300 dark:border-gray-600 dark:hover:text-white dark:hover:bg-gray-700 dark:focus:ring-gray-700"
                        >
                            Close
                        </Dialog.CloseTrigger>
                    </div>
                {:else}
                    <p class="mb-4 text-gray-900 dark:text-white">
                        {aliasStep === 'near'
                            ? 'Bring the device close to this node and click Detect.'
                            : 'Move the device far away and click Detect.'}
                    </p>
                    {#if aliasCandidates.length > 1}
                        <p class="mb-4 text-sm text-gray-600 dark:text-gray-400">{aliasCandidates.length} devices remaining</p>
                    {:else if aliasCandidates.length === 0 && aliasStep !== 'near'}
                        <p class="mb-4 text-sm text-red-600">No devices detected. Try again.</p>
                    {/if}
                    <div class="flex justify-end space-x-2">
                        <button
                            class="text-white bg-blue-600 hover:bg-blue-700 focus:ring-4 focus:outline-none focus:ring-blue-300 text-center dark:bg-blue-500 dark:hover:bg-blue-600 dark:focus:ring-blue-800"
                            onclick={detect}
                        >
                            Detect
                        </button>
                        <Dialog.CloseTrigger
                            type="button"
                            class="text-gray-500 bg-white hover:bg-gray-100 focus:ring-4 focus:outline-none focus:ring-blue-300 border border-gray-200 hover:text-gray-900 focus:z-10 dark:bg-gray-800 dark:text-gray-300 dark:border-gray-600 dark:hover:text-white dark:hover:bg-gray-700 dark:focus:ring-gray-700"
                        >
                            Cancel
                        </Dialog.CloseTrigger>
                    </div>
                {/if}
            </Dialog.Content>
        </Dialog.Positioner>
    </Dialog>
{/if}

<Dialog open={showEditModal} onOpenChange={handleEditOpenChange} modal={false} preventScroll={true}>
    <Dialog.Backdrop class="fixed inset-0 bg-gray-500/75 transition-opacity" />
    <Dialog.Positioner class="fixed inset-0 flex items-center justify-center w-full h-full p-4">
        <Dialog.Content class="relative bg-white dark:bg-gray-800 rounded-lg shadow-xl max-w-lg w-full">
            <div class="flex justify-between items-start p-4 border-b dark:border-gray-700">
                <Dialog.Title class="text-xl font-semibold text-gray-900 dark:text-white">Edit Device</Dialog.Title>
                <Dialog.CloseTrigger
                    type="button"
                    class="text-gray-400 bg-transparent hover:bg-gray-200 hover:text-gray-900 p-2 ml-auto inline-flex items-center dark:hover:bg-gray-700 dark:hover:text-white"
                >
                    <span class="sr-only">Close</span>
                    <svg class="w-5 h-5" fill="currentColor" viewBox="0 0 20 20" xmlns="http://www.w3.org/2000/svg"><path fill-rule="evenodd" d="M4.293 4.293a1 1 0 011.414 0L10 8.586l4.293-4.293a1 1 0 111.414 1.414L11.414 10l4.293 4.293a1 1 0 01-1.414 1.414L10 11.414l-4.293 4.293a1 1 0 01-1.414-1.414L8.586 10 4.293 5.707a1 1 0 010-1.414z" clip-rule="evenodd"></path></svg>
                </Dialog.CloseTrigger>
            </div>
            {#if editingConfig}
                <div class="p-6 space-y-6">
                    <div class="space-y-4">
                        <div>
                            <label for="edit-device-id" class="block text-sm font-medium text-gray-700 dark:text-gray-300">ID</label>
                            <input
                                id="edit-device-id"
                                class="mt-1 block w-full border-gray-300 dark:border-gray-600 dark:bg-gray-700 dark:text-white shadow-sm focus:border-blue-500 focus:ring-blue-500"
                                type="text"
                                bind:value={editingConfig.id}
                                readonly
                            />
                        </div>
                        <div>
                            <label for="edit-device-alias" class="block text-sm font-medium text-gray-700 dark:text-gray-300">Alias</label>
                            <input
                                id="edit-device-alias"
                                class="mt-1 block w-full border-gray-300 dark:border-gray-600 dark:bg-gray-700 dark:text-white shadow-sm focus:border-blue-500 focus:ring-blue-500"
                                type="text"
                                bind:value={editingConfig.alias}
                            />
                        </div>
                        <div>
                            <label for="edit-device-name" class="block text-sm font-medium text-gray-700 dark:text-gray-300">Name</label>
                            <input
                                id="edit-device-name"
                                class="mt-1 block w-full border-gray-300 dark:border-gray-600 dark:bg-gray-700 dark:text-white shadow-sm focus:border-blue-500 focus:ring-blue-500"
                                type="text"
                                bind:value={editingConfig.name}
                            />
                        </div>
                        <div>
                            <label for="edit-device-rssi" class="block text-sm font-medium text-gray-700 dark:text-gray-300">RSSI at 1m (dBm)</label>
                            <input
                                id="edit-device-rssi"
                                class="mt-1 block w-full border-gray-300 dark:border-gray-600 dark:bg-gray-700 dark:text-white shadow-sm focus:border-blue-500 focus:ring-blue-500"
                                type="number"
                                bind:value={editingConfig["rssi@1m"]}
                            />
                        </div>
                    </div>
                </div>
                <div class="flex items-center justify-between p-6 space-x-4 border-t border-gray-200 dark:border-gray-700">
                    <button
                        onclick={onDeleteConfig}
                        class="text-white bg-red-600 hover:bg-red-700 focus:ring-4 focus:outline-none focus:ring-red-300 text-center dark:bg-red-500 dark:hover:bg-red-600 dark:focus:ring-red-800"
                    >
                        Delete
                    </button>
                    <div class="flex items-center space-x-4">
                        <button
                            onclick={onSaveEdit}
                            class="text-white bg-blue-600 hover:bg-blue-700 focus:ring-4 focus:outline-none focus:ring-blue-300 text-center dark:bg-blue-500 dark:hover:bg-blue-600 dark:focus:ring-blue-800"
                        >
                            Save
                        </button>
                        <Dialog.CloseTrigger
                            type="button"
                            class="text-gray-500 bg-white hover:bg-gray-100 focus:ring-4 focus:outline-none focus:ring-blue-300 border border-gray-200 hover:text-gray-900 focus:z-10 dark:bg-gray-800 dark:text-gray-300 dark:border-gray-600 dark:hover:text-white dark:hover:bg-gray-700 dark:focus:ring-gray-700"
                        >
                            Cancel
                        </Dialog.CloseTrigger>
                    </div>
                </div>
            {/if}
        </Dialog.Content>
    </Dialog.Positioner>
</Dialog>
