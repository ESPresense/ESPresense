<script>
    import { devices, events, enroll } from "../stores";
    let name;

    function slugify(name) {
        return name
            .toString()
            .normalize("NFD") // split an accented letter in the base letter and the acent
            .replace(/[\u0300-\u036f]/g, "") // remove all previously split accents
            .toLowerCase()
            .trim()
            .replace(/[^a-z0-9 ]/g, "") // remove all chars not letters, numbers and spaces (to be replaced)
            .replace(/\s+/g, "-"); // separator
    }
</script>

{#if $events?.state != null}
    <label for="name">Name</label>
    <input id="name" type="text" class="bg-gray-200 relative" placeholder="Name" bind:value={name} />
    <button class="bg-blue-500 hover:bg-blue-700 text-white font-bold py-2 px-4 rounded disabled:opacity-50 disabled:cursor-not-allowed" disabled={$events.state.enrolling} on:click={() => enroll(name)}> Enroll </button>

    {#if $events?.state?.enrolling}
        <p>
            To begin, navigate to your device's Bluetooth settings and pair with the ESPresense device. After obtaining the IRK, ESPresense will disconnect; there's no need to maintain the pairing on your device. Your device will display the name: <span class="bg-gray-400 accent-inherit relative whitespace-nowrap">{slugify(name)}</span> on both the fingerprint page and MQTT. This configuration is saved as a retained message in MQTT and will be loaded by all ESPresense devices on the same MQTT server.
        </p>
        <p>
            Start the pairing process now.
        </p>
    {/if}
{/if}
