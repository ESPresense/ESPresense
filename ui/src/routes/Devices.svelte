<script>
    import { devices, events, enroll, cancelEnroll } from "../stores";
    let name;
    let showModal = false;

    function onEnroll() {
        enroll(name);
    }

    function onCancel() {
        showModal = false;
        cancelEnroll();
    }

    function formatRemainingTime(seconds) {
        const minutes = Math.floor(seconds / 60);
        const remainingSeconds = seconds % 60;
        return [minutes, remainingSeconds].map((val) => val.toString().padStart(2, "0")).join(":");
    }

    $: if ($events?.state?.enrolling) showModal = true;
</script>

<button on:click={() => (showModal = true)} class="text-white bg-blue-700 hover:bg-blue-800 focus:ring-4 focus:outline-none focus:ring-blue-300 font-medium rounded-lg text-sm px-5 py-2.5 text-center dark:bg-blue-600 dark:hover:bg-blue-700 dark:focus:ring-blue-800">Enroll</button>
{#if showModal}
    <div class="fixed inset-0 bg-gray-500 bg-opacity-75 transition-opacity">
        <div class="flex items-center justify-center w-full h-full">
            <div class="relative bg-white rounded-lg shadow dark:bg-gray-700 min-w-lg sm:max-w-lg sm:w-full mx-auto">
                <div class="flex justify-between items-start p-4 border-b dark:border-gray-600">
                    <h3 class="text-xl font-semibold text-gray-900 dark:text-white">Enroll Device - Office</h3>
                    <button type="button" class="text-gray-400 bg-transparent hover:bg-gray-200 hover:text-gray-900 rounded-lg text-sm p-2 ml-auto inline-flex items-center justify-center dark:hover:bg-gray-600 dark:hover:text-white" on:click={onCancel}>
                        <span class="sr-only">Close</span>
                    </button>
                </div>
                <div class="p-6 space-y-6">
                    {#if !$events?.state?.enrolling}
                        {#if $events?.state?.enrolledId}
                            <p class="text-sm leading-5 text-gray-500">
                                The device has been enrolled. ESPresense has disconnected; you can delete the pairing on your device. The new device ID is:
                                <span class="font-semibold">{$events.state.enrolledId}</span>.
                            </p>
                        {:else}
                            <div class="mt-3 text-center sm:mt-0 sm:text-left">
                                <input class="border p-2 w-full" type="text" bind:value={name} placeholder="Enter name" />
                            </div>
                        {/if}
                    {:else}
                        <p class="text-sm leading-5 text-gray-500">To begin, navigate to your device's Bluetooth settings (on Apple Watch use the BluetoothLE app) and pair with the ESPresense device.</p>
                        <p class="mt-4 text-sm leading-5 text-gray-500">Start the pairing process now. Time remaining: {formatRemainingTime($events.state.remain)}</p>
                    {/if}
                </div>
                <div class="items-center p-6 space-x-4 space-x-reverse border-t border-gray-200 dark:border-gray-600 sm:px-6 sm:flex sm:flex-row-reverse">
                    {#if !$events?.state?.enrolling && !$events?.state?.enrolledId}
                        <button on:click={onEnroll} class="text-white bg-blue-700 hover:bg-blue-800 focus:ring-4 focus:outline-none focus:ring-blue-300 font-medium rounded-lg text-sm px-5 py-2.5 text-center dark:bg-blue-600 dark:hover:bg-blue-700 dark:focus:ring-blue-800">Enroll</button>
                    {/if}
                    <button on:click={onCancel} class="text-gray-500 bg-white hover:bg-gray-100 focus:ring-4 focus:outline-none focus:ring-blue-300 rounded-lg border border-gray-200 text-sm font-medium px-5 py-2.5 hover:text-gray-900 focus:z-10 dark:bg-gray-700 dark:text-gray-300 dark:border-gray-500 dark:hover:text-white dark:hover:bg-gray-600 dark:focus:ring-gray-600">Close</button>
                </div>
            </div>
        </div>
    </div>
{/if}
