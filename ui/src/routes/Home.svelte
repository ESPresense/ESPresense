<script>
  import { devices, events, enroll } from "../stores";
  import Stats from "../components/Stats.svelte";
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

<Stats />
{#if $events?.state != null}
  <label for="name">Name</label>
  <input id="name" type="text" class="bg-gray-200 relative" placeholder="Name" bind:value={name} />
  <button class="bg-blue-500 hover:bg-blue-700 text-white font-bold py-2 px-4 rounded disabled:opacity-50 disabled:cursor-not-allowed" disabled={$events.state.enrolling} on:click={() => enroll(name)}> Enroll </button>

  {#if $events?.state?.enrolling}
    <p>
      You can now use the Bluetooth settings on your device and pair it with the ESPresense device. Once paired your device will be shown with the name: <span class="bg-gray-200 relative whitespace-nowrap">{slugify(name)}</span>
      in the fingerprint page as well as on mqtt.
    </p>
  {/if}
{/if}
