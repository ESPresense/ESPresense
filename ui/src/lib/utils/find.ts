// Find mode: pick out one fingerprint by moving the device near the node and away again.
//
// The node reports each fingerprint's RSSI as a median over a sliding window (rssi_window_ms,
// 15 s by default), so a step has to be held long enough for that median to swing. Each step
// keeps every poll's reading; a step's level is the median of its last few readings, once the
// window has caught up. A device scores on every near/far boundary it was seen on both sides
// of: near level minus far level. The thing being moved gains a few dB on every boundary;
// everything else drifts both ways and averages out.

import type { Device } from '../types.js';

export type Phase = 'near' | 'far';

export interface Step {
    phase: Phase;
    startedAt: number;
    readings: Map<string, number[]>;
    /** the last row seen for each key, so a device that drops out can still be ranked */
    rows: Map<string, Device>;
}

export interface Candidate {
    key: string;
    device: Device;
    /** mean dB gained near over far, across boundaries */
    score: number;
    /** boundaries this device was seen on both sides of */
    boundaries: number;
    /** every boundary went the right way by at least MIN_SWING */
    consistent: boolean;
}

/** readings per step that count towards its level (the tail, after the window caught up); fewer is no level */
export const TAIL = 3;
/** dB a boundary must swing to count as following the user's move */
export const MIN_SWING = 3;
/** how long to hold a step before the window median has moved (half of the 15 s default) */
export const MIN_HOLD_MS = 8000;

/**
 * One fingerprint row. The MAC, not the id: ids can be shared (several iPhones are all
 * "apple:1005:9-26"), and their readings must not mix.
 */
export function rowKey(d: Device): string | undefined {
    return d.mac || d.id || undefined;
}

export function newStep(phase: Phase, now: number): Step {
    return { phase, startedAt: now, readings: new Map(), rows: new Map() };
}

export function record(step: Step, devices: Device[]): void {
    for (const d of devices) {
        const key = rowKey(d);
        if (!key || d.rssi == null || !d.vis) continue;
        step.rows.set(key, d);
        const r = step.readings.get(key);
        if (r) r.push(d.rssi);
        else step.readings.set(key, [d.rssi]);
    }
}

function median(xs: number[]): number {
    const s = [...xs].sort((a, b) => a - b);
    const m = s.length >> 1;
    return s.length % 2 ? s[m] : (s[m - 1] + s[m]) / 2;
}

/** a step's settled level for one device, or undefined if it wasn't seen enough */
export function level(step: Step, key: string): number | undefined {
    const r = step.readings.get(key);
    if (!r || r.length < TAIL) return undefined;
    return median(r.slice(-TAIL));
}

export function rank(steps: Step[], devices: Device[]): Candidate[] {
    const byKey = new Map<string, Device>();
    for (const s of steps) for (const [key, d] of s.rows) byKey.set(key, d);
    for (const d of devices) {
        const key = rowKey(d);
        if (key && byKey.has(key)) byKey.set(key, d);
    }

    const out: Candidate[] = [];
    for (const [key, device] of byKey) {
        let sum = 0;
        let boundaries = 0;
        let consistent = true;
        for (let i = 1; i < steps.length; i++) {
            const a = level(steps[i - 1], key);
            const b = level(steps[i], key);
            if (a === undefined || b === undefined) continue;
            const swing = steps[i].phase === 'near' ? b - a : a - b;
            sum += swing;
            boundaries++;
            if (swing < MIN_SWING) consistent = false;
        }
        if (boundaries === 0) continue;
        out.push({ key, device, score: sum / boundaries, boundaries, consistent });
    }

    return out.sort(
        (a, b) =>
            Number(b.consistent) - Number(a.consistent) ||
            b.boundaries - a.boundaries ||
            b.score - a.score
    );
}

/**
 * The winner, once the evidence is in: near-far-near (two boundaries) all the right way, a
 * clear swing, and nothing else that also followed every move.
 */
export function winner(ranked: Candidate[], boundaries: number): Candidate | undefined {
    if (boundaries < 2) return undefined;
    const [top, next] = ranked;
    if (!top || !top.consistent || top.boundaries < boundaries || top.score < 2 * MIN_SWING) return undefined;
    if (next && next.consistent && next.boundaries >= boundaries) return undefined;
    return top;
}
