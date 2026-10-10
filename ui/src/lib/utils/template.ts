/**
 * Board templates (#2529): a JSON document of hardware settings for a specific board.
 *
 * ```json
 * { "name": "Athom Smart Plug V3", "chip": "esp32c3", "settings": { "led_1_pin": 6 } }
 * ```
 *
 * The node validates and applies them (POST /json/template); only the keys in `settings` change.
 * Any setting may appear (pins, LEDs, Ethernet type, ...), so templates live on their own page.
 */

export interface TemplateChange {
	key: string;
	label: string;
	from: unknown;
	to: unknown;
}

export interface TemplateResult {
	changes: TemplateChange[];
	restart: boolean;
}

/**
 * Parses pasted template text, returning it re-serialized compactly (the node caps the body size).
 * @throws Error with a user-facing message when the text is not a template-shaped JSON object
 */
export function normalizeTemplate(text: string): string {
	let parsed: unknown;
	try {
		parsed = JSON.parse(text);
	} catch (error) {
		throw new Error(`Not valid JSON: ${(error as Error).message}`);
	}
	if (parsed === null || typeof parsed !== 'object' || Array.isArray(parsed)) {
		throw new Error('A template must be a JSON object with "chip" and "settings"');
	}
	return JSON.stringify(parsed);
}

/**
 * Sends a template to the node. With `dryRun` the node only validates it and reports what would
 * change; otherwise it saves the changes and restarts if there were any.
 * @throws Error carrying the node's validation message
 */
export async function postTemplate(text: string, dryRun: boolean): Promise<TemplateResult> {
	const body = normalizeTemplate(text);
	const response = await fetch(dryRun ? '/json/template?dry' : '/json/template', {
		method: 'POST',
		headers: { 'Content-Type': 'application/json' },
		body
	});
	let data: { error?: string } & Partial<TemplateResult>;
	try {
		data = await response.json();
	} catch {
		throw new Error(`Unexpected response from node (${response.status})`);
	}
	if (!response.ok || data.error) {
		throw new Error(data.error ?? `Request failed (${response.status})`);
	}
	return { changes: data.changes ?? [], restart: data.restart ?? false };
}

/**
 * Waits for the node to come back after applying a template restarted it.
 * @throws Error if it doesn't answer within about 20 seconds
 */
export async function waitForRestart(attempts = 15): Promise<void> {
	await new Promise((resolve) => setTimeout(resolve, 2000));
	for (let i = 0; i < attempts; i++) {
		try {
			const response = await fetch('/json/template', { signal: AbortSignal.timeout(2000) });
			if (response.ok) return;
		} catch {
			// still restarting
		}
		await new Promise((resolve) => setTimeout(resolve, 1000));
	}
	throw new Error('The node did not come back after restarting. Please refresh the page.');
}

/** Fetches this node's board settings as a template. */
export async function fetchTemplate(): Promise<Record<string, unknown>> {
	const response = await fetch('/json/template');
	if (!response.ok) throw new Error(`Export failed (${response.status})`);
	return response.json();
}

/** Display form for a setting value in the change preview. */
export function formatValue(value: unknown): string {
	if (value === '' || value === undefined || value === null) return '(empty)';
	return String(value);
}
