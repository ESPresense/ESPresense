<script>
    import { goto } from '$app/navigation';
    import { resolve } from '$app/paths';
    import { onMount } from 'svelte';

    onMount(() => {
        goto(resolve('/network'));
    });
</script>

<div>Redirecting to settings...</div>
