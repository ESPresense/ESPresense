<script lang="ts">
    import { hardwareSettings } from '#lib/stores.js';
    import { reloadSettingsWithRetry } from '#lib/utils/settings.js';
    import { fetchTemplate, formatValue, postTemplate, type TemplateChange } from '#lib/utils/template.js';

    let text = $state<string>('');
    /** Changes from the last successful preview; null until previewed */
    let changes = $state<TemplateChange[] | null>(null);
    /** The text that produced `changes`: editing it afterwards requires a new preview */
    let previewedText = $state<string>('');
    let error = $state<string>('');
    let status = $state<string>('');
    let busy = $state<boolean>(false);

    const canApply = $derived(changes !== null && changes.length > 0 && previewedText === text && !busy);

    async function preview(): Promise<void> {
        error = '';
        status = '';
        changes = null;
        busy = true;
        try {
            const result = await postTemplate(text, true);
            changes = result.changes;
            previewedText = text;
        } catch (e) {
            error = (e as Error).message;
        } finally {
            busy = false;
        }
    }

    async function apply(): Promise<void> {
        error = '';
        busy = true;
        try {
            const result = await postTemplate(text, false);
            if (!result.restart) {
                status = 'Nothing to change.';
                return;
            }
            status = 'Template applied. Restarting...';
            changes = null;
            text = '';
            await new Promise((resolve) => setTimeout(resolve, 2000));
            await reloadSettingsWithRetry('/wifi/hardware', hardwareSettings, 15);
            status = 'Template applied.';
        } catch (e) {
            error = (e as Error).message;
        } finally {
            busy = false;
        }
    }

    async function exportTemplate(): Promise<void> {
        error = '';
        try {
            const template = await fetchTemplate();
            const blob = new Blob([JSON.stringify(template, null, 2) + '\n'], { type: 'application/json' });
            const url = URL.createObjectURL(blob);
            const a = document.createElement('a');
            const name = typeof template.name === 'string' && template.name ? template.name : 'espresense';
            a.href = url;
            a.download = `${name.replace(/[^\w.-]+/g, '-')}-template.json`;
            a.click();
            URL.revokeObjectURL(url);
        } catch (e) {
            error = (e as Error).message;
        }
    }
</script>

<section id="board-template" class="space-y-4">
    <h2>
        <a href="https://espresense.com/configuration/hardware#board-templates" target="_blank">Board Template</a>
    </h2>
    <p class="text-sm text-gray-600 dark:text-gray-300">
        Paste a board template to set this board's pins in one step. Only the settings in the template change;
        the node restarts after applying.
    </p>
    <label>
        Template JSON:<br />
        <textarea
            name="template"
            rows="6"
            spellcheck="false"
            placeholder={'{"name": "...", "chip": "esp32", "settings": {"led_1_pin": 2}}'}
            class="block w-full rounded-xl border-2 border-gray-300 bg-white px-4 py-3 font-mono text-sm focus:border-blue-500 focus:ring-blue-500 dark:border-gray-500 dark:bg-gray-900 dark:text-white"
            bind:value={text}></textarea>
    </label>

    {#if error}
        <p role="alert" class="text-red-600 dark:text-red-400">{error}</p>
    {/if}
    {#if status}
        <p role="status" class="text-green-700 dark:text-green-400">{status}</p>
    {/if}

    {#if changes !== null && previewedText === text}
        {#if changes.length === 0}
            <p role="status">This board already matches the template.</p>
        {:else}
            <table class="w-full text-sm" data-testid="template-changes">
                <thead>
                    <tr class="text-left">
                        <th class="py-1 pr-4">Setting</th>
                        <th class="py-1 pr-4">Current</th>
                        <th class="py-1">Template</th>
                    </tr>
                </thead>
                <tbody>
                    {#each changes as change (change.key)}
                        <tr class="border-t border-gray-300 dark:border-gray-600">
                            <td class="py-1 pr-4" title={change.label}><code>{change.key}</code></td>
                            <td class="py-1 pr-4">{formatValue(change.from)}</td>
                            <td class="py-1 font-semibold">{formatValue(change.to)}</td>
                        </tr>
                    {/each}
                </tbody>
            </table>
        {/if}
    {/if}

    <div class="flex flex-wrap justify-end gap-2">
        <button type="button" class="bg-gray-200 text-gray-900 hover:bg-gray-300 dark:bg-gray-700 dark:text-white dark:hover:bg-gray-600" onclick={exportTemplate}>
            Export as template
        </button>
        <button type="button" class="bg-gray-200 text-gray-900 hover:bg-gray-300 dark:bg-gray-700 dark:text-white dark:hover:bg-gray-600" disabled={busy || !text.trim()} onclick={preview}>
            Preview
        </button>
        <button type="button" class="text-white bg-blue-600 hover:bg-blue-700 disabled:opacity-50 dark:bg-blue-500 dark:hover:bg-blue-600" disabled={!canApply} onclick={apply}>
            {busy && changes !== null ? 'Applying...' : 'Apply'}
        </button>
    </div>
</section>
