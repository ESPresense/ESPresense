<script lang="ts">
    import SvelteTable from "svelte-table";
    import { devices, configs, enroll } from "$lib/stores";
    import type { Device, TableColumn } from "$lib/types";
    import AddDeviceButton from "./AddDeviceButton.svelte";

    let filterSelections = $state({
        vis: true
    });
    let sortBy = $state("distance");
    let sortOrder = $state<-1 | 0 | 1>(1);
    let selectedRowIds = $state([]);
    let findMode = $state(false);
    let findPhase = $state<'close' | 'far'>('close'); // Current phase: bring close or move far
    let signalHistory = $state(new Map<string, number[]>());
    let candidateDevices = $state(new Set<string>()); // MAC addresses of candidate devices
    let phaseStartTime = $state(0); // Track when current phase started

    // Device creation modal state
    let showEnrollModal = $state(false);
    let deviceName = $state("");
    let deviceId = $state("");
    let deviceType = $state("");
    let selectedDevice = $state<Device | null>(null);

    const deviceTypes = ["watch", "wallet", "ipad", "phone", "airpods", "laptop", "node", "keys", "therm", "flora", "tile"];

    function generateKebabCaseId(name: string, type = "") {
        const words = name.toLowerCase().split(/\s+/);
        const filteredWords = words.filter((word) => word !== type.toLowerCase());
        return filteredWords
            .join("-")
            .replace(/[']/g, "")
            .replace(/[^a-z0-9-]+/g, "-")
            .replace(/(^-|-$)/g, "");
    }

    function addDevice(device: Device) {
        selectedDevice = device;

        // Pre-populate form with device data
        deviceName = device.name || "";
        deviceId = device.mac || "";
        deviceType = "";

        showEnrollModal = true;
    }

    function onEnroll() {
        if (!deviceName) return;

        const generatedId = deviceType ? `${deviceType}:${generateKebabCaseId(deviceName, deviceType)}` : generateKebabCaseId(deviceName);
        enroll(deviceId || generatedId, deviceName);

        // Close modal and reset form
        showEnrollModal = false;
        resetEnrollForm();
    }

    function resetEnrollForm() {
        deviceName = "";
        deviceId = "";
        deviceType = "";
        selectedDevice = null;
    }

    function onSelectExisting(event: Event) {
        const alias = (event.target as HTMLSelectElement).value;
        if (!alias) {
            deviceId = "";
            deviceName = "";
            return;
        }
        const configRows = $configs?.configs || [];
        const cfg = configRows.find((c) => c.alias === alias);
        if (cfg) {
            deviceId = cfg.alias || "";
            deviceName = cfg.name || "";
        }
    }

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
        {
            key: "actions",
            title: "Actions",
            value: () => "",
            renderComponent: {
                component: AddDeviceButton,
                props: {
                    onAdd: addDevice
                }
            },
            sortable: false,
            headerClass: "px-6 py-4 text-left text-xs font-medium text-gray-700 dark:text-gray-200 uppercase tracking-wider",
            class: "px-6 py-4 whitespace-nowrap",
        },
    ];

    // Manage candidate devices for Find Mode
    function updateCandidates(devices: Device[]) {
        if (!findMode) {
            candidateDevices.clear();
            return;
        }

        // If no candidates yet, start with all close devices
        if (candidateDevices.size === 0) {
            const newCandidates = new Set<string>();
            devices.forEach(device => {
                if (device.close && device.mac) {
                    newCandidates.add(device.mac);
                }
            });
            candidateDevices = newCandidates;
            return;
        }

        // Track signal history for candidate devices
        devices.forEach(device => {
            if (device.mac && candidateDevices.has(device.mac) && device.rssi !== undefined) {
                const history = signalHistory.get(device.mac) || [];
                history.push(device.rssi);

                // Keep only last 8 readings (8 seconds of history)
                if (history.length > 8) {
                    history.shift();
                }

                signalHistory.set(device.mac, history);
            }
        });
    }

    // Check if device is a candidate based on current phase behavior
    function isCandidateDevice(device: Device): boolean {
        if (!findMode || !device.mac || !candidateDevices.has(device.mac)) {
            return false;
        }

        const history = signalHistory.get(device.mac);
        if (!history || history.length < 5) return true; // Show all candidates initially

        // Get signal readings from current phase (since phaseStartTime)
        const currentTime = Date.now();
        const phaseHistory = history.slice(Math.max(0, history.length - Math.floor((currentTime - phaseStartTime) / 1000)));

        if (phaseHistory.length < 3) return true; // Not enough data yet

        const latestSignal = phaseHistory[phaseHistory.length - 1];
        const phaseStartSignal = phaseHistory[0];
        const signalChange = latestSignal - phaseStartSignal;

        if (findPhase === 'close') {
            // In "bring close" phase: device should show stronger signal (less negative RSSI)
            return signalChange > 2; // Signal improved by at least 2 dBm
        } else {
            // In "move far" phase: device should show weaker signal (more negative RSSI)
            return signalChange < -2; // Signal weakened by at least 2 dBm
        }
    }

    // Switch between close and far phases
    function switchPhase() {
        findPhase = findPhase === 'close' ? 'far' : 'close';
        phaseStartTime = Date.now();

        // Filter out devices that didn't respond to the previous phase
        const currentCandidates = new Set<string>();
        candidateDevices.forEach(mac => {
            const device = tableRows.find(d => d.mac === mac);
            if (device && isCandidateDevice(device)) {
                currentCandidates.add(mac);
            }
        });
        candidateDevices = currentCandidates;
    }

    // Clear candidates when Find Mode is toggled off
    function onFindModeToggle() {
        if (!findMode) {
            candidateDevices.clear();
            signalHistory.clear();
        } else {
            // Starting Find Mode - reset to initial state
            findPhase = 'close';
            phaseStartTime = Date.now();
        }
    }

    function classNameRow(d: Device) {
        // Priority: Orange (Find Mode candidates) > Yellow (close) > Default
        if (findMode && isCandidateDevice(d)) {
            return "bg-orange-200 dark:bg-orange-800";
        }
        if (d.close) {
            return "bg-yellow-100 dark:bg-yellow-900";
        }
        return "";
    }

    let tableRows = $derived($devices?.devices || []);

    // Update candidates when devices change
    $effect(() => {
        if ($devices?.devices) {
            updateCandidates($devices.devices);
        }
    });

    // Handle modal keyboard events
    $effect(() => {
        if (showEnrollModal && typeof document !== 'undefined') {
            const handleKeyDown = (e: KeyboardEvent) => {
                if (e.key === 'Escape') {
                    showEnrollModal = false;
                }
            };

            document.addEventListener('keydown', handleKeyDown);
            return () => {
                document.removeEventListener('keydown', handleKeyDown);
            };
        }
    });

</script>

<div class="bg-gray-100 dark:bg-gray-800 rounded-lg shadow">
    {#if $devices != null}
        <div class="p-6">
            <div class="flex justify-between items-center mb-6">
                <h2 class="text-xl font-semibold text-gray-900 dark:text-white">Fingerprints</h2>
                <label class="inline-flex items-center cursor-pointer">
                    <input
                        type="checkbox"
                        bind:checked={findMode}
                        onchange={onFindModeToggle}
                        class="sr-only peer"
                    />
                    <div class="relative w-11 h-6 bg-gray-200 peer-focus:outline-none peer-focus:ring-4 peer-focus:ring-blue-300 dark:peer-focus:ring-blue-800 rounded-full peer dark:bg-gray-700 peer-checked:after:translate-x-full rtl:peer-checked:after:-translate-x-full peer-checked:after:border-white after:content-[''] after:absolute after:top-[2px] after:start-[2px] after:bg-white after:border-gray-300 after:border after:rounded-full after:h-5 after:w-5 after:transition-all dark:border-gray-600 peer-checked:bg-blue-600"></div>
                    <span class="ms-3 text-sm font-medium text-gray-900 dark:text-gray-300">Find Mode</span>
                </label>
            </div>

            <!-- Find Mode Status -->
            {#if findMode}
                <div class="mb-4 p-4 bg-blue-50 dark:bg-blue-900/20 border border-blue-200 dark:border-blue-800 rounded-md">
                    <div class="flex items-start justify-between">
                        <div class="flex-1">
                            <div class="flex items-center gap-2 mb-3">
                                <span class="px-2 py-1 text-xs font-medium rounded-full {findPhase === 'close' ? 'bg-green-100 text-green-800 dark:bg-green-900/30 dark:text-green-300' : 'bg-red-100 text-red-800 dark:bg-red-900/30 dark:text-red-300'}">
                                    {findPhase === 'close' ? '📱 Bring Close' : '🚶 Move Far'}
                                </span>
                            </div>
                            <p class="text-xs text-blue-600 dark:text-blue-300 mb-3">
                                {#if candidateDevices.size === 0}
                                    Waiting for close devices to appear...
                                {:else if candidateDevices.size > 1}
                                    {candidateDevices.size} candidates found.
                                    {#if findPhase === 'close'}
                                        Bring your target device close, then click "Switch to Move Far".
                                    {:else}
                                        Move your target device further away, then click "Switch to Bring Close" or "Done" if you're confident.
                                    {/if}
                                {:else if candidateDevices.size === 1}
                                    🎯 1 candidate identified! You can now add this device.
                                {:else}
                                    All devices filtered out. Toggle Find Mode off and on to start over.
                                {/if}
                            </p>

                            <!-- Success Message -->
                            {#if candidateDevices.size === 1}
                                <div class="px-3 py-1 text-xs font-medium text-green-700 dark:text-green-300 bg-green-100 dark:bg-green-900/30 rounded">
                                    ✅ Device identified! Use + Add button.
                                </div>
                            {/if}
                        </div>

                        <!-- Phase Control Button -->
                        {#if candidateDevices.size > 1}
                            <button
                                type="button"
                                onclick={switchPhase}
                                class="px-3 py-1 text-xs font-medium text-white bg-blue-600 hover:bg-blue-700 rounded focus:outline-none focus:ring-2 focus:ring-blue-500 ml-2"
                            >
                                {findPhase === 'close' ? 'Switch to Move Far' : 'Switch to Bring Close'}
                            </button>
                        {/if}
                    </div>
                </div>
            {/if}

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

<!-- Enrollment Modal -->
{#if showEnrollModal}
    <div class="fixed inset-0 bg-gray-600 bg-opacity-50 overflow-y-auto h-full w-full z-50" role="dialog" aria-modal="true" aria-labelledby="modal-title" tabindex="-1">
        <div class="relative top-20 mx-auto p-5 border w-96 shadow-lg rounded-md bg-white dark:bg-gray-800">
            <div class="mt-3">
                <h3 id="modal-title" class="text-lg font-medium text-gray-900 dark:text-white mb-4">
                    Add Device from Fingerprint
                </h3>

                <form onsubmit={onEnroll} class="space-y-4">
                    <!-- Existing Device Selection -->
                    <div>
                        <label for="existing-device" class="block text-sm font-medium text-gray-700 dark:text-gray-300">Replace existing device (optional)</label>
                        <select id="existing-device" onchange={onSelectExisting} class="mt-1 block w-full border-gray-300 dark:border-gray-600 dark:bg-gray-700 dark:text-white rounded-md shadow-sm focus:border-blue-500 focus:ring-blue-500">
                            <option value="">Select existing ID</option>
                            {#each $configs?.configs || [] as config}
                                <option value={config.alias}>{config.alias} - {config.name}</option>
                            {/each}
                        </select>
                    </div>

                    <!-- Device Type -->
                    <div>
                        <label for="device-type" class="block text-sm font-medium text-gray-700 dark:text-gray-300">Device type</label>
                        <select id="device-type" bind:value={deviceType} class="mt-1 block w-full border-gray-300 dark:border-gray-600 dark:bg-gray-700 dark:text-white rounded-md shadow-sm focus:border-blue-500 focus:ring-blue-500">
                            <option value="">Select device type</option>
                            {#each deviceTypes as type}
                                <option value={type}>{type}</option>
                            {/each}
                        </select>
                    </div>

                    <!-- Device Name -->
                    <div>
                        <label for="device-name" class="block text-sm font-medium text-gray-700 dark:text-gray-300">Device name</label>
                        <input
                            id="device-name"
                            type="text"
                            bind:value={deviceName}
                            placeholder="Enter name"
                            required
                            class="mt-1 block w-full border-gray-300 dark:border-gray-600 dark:bg-gray-700 dark:text-white rounded-md shadow-sm focus:border-blue-500 focus:ring-blue-500"
                        />
                    </div>

                    <!-- Device ID -->
                    <div>
                        <label for="device-id" class="block text-sm font-medium text-gray-700 dark:text-gray-300">Device ID</label>
                        <input
                            id="device-id"
                            type="text"
                            bind:value={deviceId}
                            placeholder={deviceType && deviceName ? `${deviceType}:${generateKebabCaseId(deviceName, deviceType)}` : "Enter custom ID or leave for auto-generated"}
                            class="mt-1 block w-full border-gray-300 dark:border-gray-600 dark:bg-gray-700 dark:text-white rounded-md shadow-sm focus:border-blue-500 focus:ring-blue-500"
                        />
                    </div>

                    <!-- Device Info from Fingerprint -->
                    {#if selectedDevice}
                        <div class="bg-gray-50 dark:bg-gray-700 p-3 rounded-md">
                            <h4 class="text-sm font-medium text-gray-900 dark:text-white mb-2">Fingerprint Data:</h4>
                            <div class="text-xs text-gray-600 dark:text-gray-400 space-y-1">
                                {#if selectedDevice.mac}<div>MAC: {selectedDevice.mac}</div>{/if}
                                {#if selectedDevice.rssi}<div>RSSI: {selectedDevice.rssi} dBm</div>{/if}
                                {#if selectedDevice.distance !== undefined}<div>Distance: {selectedDevice.distance.toFixed(2)} m</div>{/if}
                            </div>
                        </div>
                    {/if}

                    <!-- Buttons -->
                    <div class="flex justify-end space-x-3 pt-4">
                        <button
                            type="button"
                            onclick={() => showEnrollModal = false}
                            class="px-4 py-2 text-sm font-medium text-gray-700 dark:text-gray-300 bg-white dark:bg-gray-600 border border-gray-300 dark:border-gray-500 rounded-md hover:bg-gray-50 dark:hover:bg-gray-500 focus:outline-none focus:ring-2 focus:ring-blue-500"
                        >
                            Cancel
                        </button>
                        <button
                            type="submit"
                            disabled={!deviceName}
                            class="px-4 py-2 text-sm font-medium text-white bg-blue-600 border border-transparent rounded-md hover:bg-blue-700 focus:outline-none focus:ring-2 focus:ring-blue-500 disabled:opacity-50 disabled:cursor-not-allowed"
                        >
                            Enroll Device
                        </button>
                    </div>
                </form>
            </div>
        </div>
    </div>
{/if}
