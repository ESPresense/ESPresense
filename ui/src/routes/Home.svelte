<script>
import { devices, events } from '../stores';
import Stats from "../components/Stats.svelte";
</script>

<Stats />
{#if $events?.state != null }
<label for="name">Name</label>
<input id="name" type="text" class=" bg-gray-200 relative" placeholder="Name" />
<button class="bg-blue-500 hover:bg-blue-700 text-white font-bold py-2 px-4 rounded">
	Enroll
</button>
{/if}
