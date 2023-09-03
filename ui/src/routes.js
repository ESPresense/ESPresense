import Fingerprints from './routes/Fingerprints.svelte';
import Devices from './routes/Devices.svelte';
import Settings from './routes/Settings.svelte';
import NotFound from './routes/NotFound.svelte';

export default {
    '/': Settings,
    '/settings': Settings,
    '/devices': Devices,
    '/fingerprints': Fingerprints,
    // The catch-all route must always be last
    '*': NotFound
};
