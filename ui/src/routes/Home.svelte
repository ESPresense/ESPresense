<script>
import { devices, events, enroll } from '../stores';
import Stats from "../components/Stats.svelte";
let name;
</script>

<Stats />
{#if $events?.state != null }
<label for="name">Name</label>
<input id="name" type="text" class=" bg-gray-200 relative" placeholder="Name" bind:value={name} />
<button class="bg-blue-500 hover:bg-blue-700 text-white font-bold py-2 px-4 rounded disabled:opacity-50 disabled:cursor-not-allowed" disabled={$events.state.enrolling} on:click={()=>enroll(name)}>
	Enroll
</button>
{/if}
