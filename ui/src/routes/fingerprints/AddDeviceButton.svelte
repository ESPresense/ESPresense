<script lang="ts">
    import type { Device } from "$lib/types";
    
    export let row: Device;
    export let onAdd: (device: Device) => void;
</script>

<button 
    type="button"
    onclick={() => onAdd(row)}
    class="px-3 py-1 text-sm font-medium text-white bg-blue-600 hover:bg-blue-700 rounded-md focus:outline-none focus:ring-2 focus:ring-blue-500 focus:ring-offset-2"
>
    + Add
</button>