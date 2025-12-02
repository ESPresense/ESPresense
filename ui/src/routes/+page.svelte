<script>
import { goto } from '$app/navigation';
import { base } from '$app/paths';
import { onMount } from 'svelte';

onMount(() => {
    goto(`${base}/network`);
});
</script>

<div>Redirecting to settings...</div>
