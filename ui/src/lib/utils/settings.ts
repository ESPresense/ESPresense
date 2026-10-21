import type { Writable } from 'svelte/store';
import type { ExtraSettings } from '#lib/types.js';

/**
 * Saves settings to the device with automatic restart and retry logic.
 *
 * This function handles the complete save flow:
 * 1. POSTs form data to the specified endpoint; a refused save (e.g. a pin this chip can't use)
 *    throws with the device's message and nothing restarts
 * 2. Triggers a device restart (which typically fails due to connection drop)
 * 3. Retries loading settings up to 5 times with 1-second delays
 * 4. Updates the provided settings store on success
 *
 * @param endpoint - The API endpoint to POST settings to (e.g., "/wifi/extras" or "/wifi/hardware")
 * @param formData - The FormData object containing settings to save
 * @param settingsStore - The Svelte store to update with reloaded settings
 * @throws Error with the device's message if it refuses the save, or if settings fail to reload
 * after all retry attempts
 *
 * @example
 * ```typescript
 * await saveSettingsWithRetry("/wifi/hardware", formData, hardwareSettings);
 * ```
 */
export async function saveSettingsWithRetry(
	endpoint: string,
	formData: FormData,
	settingsStore: Writable<ExtraSettings | null>
): Promise<void> {
	// Convert FormData to URLSearchParams for posting
	const params = new URLSearchParams();
	for (const [key, value] of formData.entries()) {
		if (typeof value === 'string') {
			params.append(key, value);
		}
	}

	// Save settings to device
	const response = await fetch(endpoint, { method: 'POST', body: params });
	if (!response.ok) {
		const message = (await response.text().catch(() => '')).trim();
		throw new Error(message || `Save failed (${response.status})`);
	}

	try {
		// Trigger device restart
		// Note: This request typically fails because the device drops the connection
		// during restart. This is expected behavior and the error is ignored.
		await fetch('/restart', { method: 'POST', signal: AbortSignal.timeout(1000) });
	} catch (error) {
		// Restart request often fails - this is expected behavior
		// The device is restarting and drops the connection
	}

	await reloadSettingsWithRetry(endpoint, settingsStore);
}

/**
 * Reloads settings into a store while the device restarts, retrying once a second.
 *
 * @param endpoint - The settings endpoint to GET (e.g. "/wifi/hardware")
 * @param settingsStore - The Svelte store to update with the reloaded settings
 * @param retries - Attempts before giving up; the device takes a few seconds to come back
 * @throws Error if settings fail to reload after all retry attempts
 */
export async function reloadSettingsWithRetry(
	endpoint: string,
	settingsStore: Writable<ExtraSettings | null>,
	retries = 5
): Promise<void> {
	while (retries > 0) {
		try {
			const response = await fetch(endpoint);
			const data = await response.json();
			settingsStore.set(data);
			return; // Success - exit function
		} catch (error) {
			retries--;
			if (retries === 0) {
				console.error('Failed to reload settings after multiple retries:', error);
				throw new Error('Failed to reload settings. Please refresh the page.');
			}
			// Wait 1 second before retrying
			await new Promise((resolve) => setTimeout(resolve, 1000));
		}
	}
}
