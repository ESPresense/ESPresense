// Find mode: pick out one fingerprint by holding the device next to the node and taking it
// away again.
//
// The node already decides "close" for every fingerprint (its median RSSI over rssi_window_ms,
// with hysteresis: enter above -40 dBm, leave below -50), so a step only has to record whether
// each device was close when the step ended. The thing being moved is close on every near
// step and not on any far step; nothing else is.

import type { Device } from '../types.js';
import { unstableId } from './alias.js';

export type Phase = 'near' | 'far';

export interface Step {
    phase: Phase;
    startedAt: number;
    /** whether each device was close on the last poll of the step */
    close: Map<string, boolean>;
    /** the last row seen for each key, so a device that drops out can still be listed */
    rows: Map<string, Device>;
}

export interface Candidate {
    key: string;
    device: Device;
    /** close on each step so far */
    close: boolean[];
    /** steps it was where the user had it: close when near, away when far */
    matched: number;
}

/**
 * Close enough to be the thing held at the node. The node's own close flag needs -40 dBm,
 * which a phone touching an Atom S3 (-44) never reaches, so its leave threshold is used here.
 */
export const NEAR_DBM = -50;
/** how long to hold a step before the node's median RSSI has caught up (rssi_window_ms, 15 s by default) */
export const MIN_HOLD_MS = 15000;

/**
 * One fingerprint row. The id when it names one device (an IRK phone keeps its id while its
 * MAC rotates every few minutes); otherwise the MAC, since shared ids like "apple:1005:9-26"
 * cover several iPhones whose readings must not mix.
 */
export function rowKey(d: Device): string | undefined {
    if (d.id && !unstableId(d.id)) return d.id;
    return d.mac || d.id || undefined;
}

export function newStep(phase: Phase, now: number): Step {
    return { phase, startedAt: now, close: new Map(), rows: new Map() };
}

export function record(step: Step, devices: Device[]): void {
    for (const d of devices) {
        const key = rowKey(d);
        if (!key || !d.vis) continue;
        step.rows.set(key, d);
        step.close.set(key, !!d.close || (d.rssi ?? -Infinity) >= NEAR_DBM);
    }
}

/** everything that was ever close on a near step, best match first */
export function rank(steps: Step[], devices: Device[]): Candidate[] {
    const byKey = new Map<string, Device>();
    for (const s of steps) for (const [key, d] of s.rows) byKey.set(key, d);
    for (const d of devices) {
        const key = rowKey(d);
        if (key && byKey.has(key)) byKey.set(key, d);
    }

    const out: Candidate[] = [];
    for (const [key, device] of byKey) {
        const close = steps.map((s) => s.close.get(key) ?? false);
        if (!steps.some((s, i) => s.phase === 'near' && close[i])) continue;
        const matched = steps.filter((s, i) => close[i] === (s.phase === 'near')).length;
        out.push({ key, device, close, matched });
    }
    return out.sort((a, b) => b.matched - a.matched);
}

/** the one device that was where the user had it on every step, once there's been near, far and near again */
export function winner(ranked: Candidate[], steps: number): Candidate | undefined {
    if (steps < 3) return undefined;
    const followed = ranked.filter((c) => c.matched === steps);
    return followed.length === 1 ? followed[0] : undefined;
}
