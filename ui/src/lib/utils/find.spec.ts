import { describe, it, expect } from 'vitest';
import { newStep, record, rank, winner, type Phase, type Step } from './find.js';
import { suggestAlias, unstableId } from './alias.js';
import { fuzzy } from './search.js';
import type { Device } from '../types.js';

const dev = (id: string, close: boolean, extra: Partial<Device> = {}): Device => ({ id, close, vis: true, ...extra });

/** one step whose last poll had these devices close or not */
function step(phase: Phase, close: Record<string, boolean>): Step {
    const s = newStep(phase, 0);
    record(s, Object.entries(close).map(([id, c]) => dev(id, c)));
    return s;
}

describe('find', () => {
    it('picks the one device that is close when near and away when far', () => {
        const steps = [
            step('near', { keys: true, tv: false, fridge: true }),
            step('far', { keys: false, tv: false, fridge: true }),
            step('near', { keys: true, tv: false, fridge: true })
        ];
        const ranked = rank(steps, []);
        expect(ranked.map((c) => c.key)).toEqual(['keys', 'fridge']);
        expect(ranked[0].close).toEqual([true, false, true]);
        expect(winner(ranked)?.key).toBe('keys');
    });

    it('waits for near, far and near again', () => {
        const steps = [step('near', { keys: true }), step('far', { keys: false })];
        expect(winner(rank(steps, []))).toBeUndefined();
    });

    it("won't choose between two devices that both followed every move", () => {
        // Keys on the same ring as a tag: both move together.
        const steps = [step('near', { keys: true, tag: true }), step('far', { keys: false, tag: false }), step('near', { keys: true, tag: true })];
        expect(winner(rank(steps, []))).toBeUndefined();
    });

    it('only lists what was close on a near step', () => {
        const steps = [step('near', { keys: true, tv: false }), step('far', { keys: false, tv: true })];
        expect(rank(steps, []).map((c) => c.key)).toEqual(['keys']);
    });

    it('takes the last poll of a step', () => {
        const s = newStep('near', 0);
        record(s, [dev('keys', false)]);
        record(s, [dev('keys', true)]);
        expect(rank([s], []).map((c) => c.key)).toEqual(['keys']);
    });

    it('counts anything at NEAR_DBM or louder as close, flag or not', () => {
        const s = newStep('near', 0);
        record(s, [dev('phone', false, { rssi: -44 }), dev('tv', false, { rssi: -70 })]);
        expect(rank([s], []).map((c) => c.key)).toEqual(['phone']);
    });

    it('follows an IRK phone across a MAC rotation', () => {
        const phone = (mac: string, c: boolean) => dev('irk:c4be', c, { mac });
        const s = (phase: Phase, mac: string, c: boolean) => {
            const st = newStep(phase, 0);
            record(st, [phone(mac, c)]);
            return st;
        };
        expect(winner(rank([s('near', 'aa', true), s('far', 'bb', false), s('near', 'bb', true)], []))?.key).toBe('irk:c4be');
    });

    it('forgives a missed first step once the last three were followed', () => {
        const steps = [
            step('near', { tag: false, other: true }),
            step('far', { tag: false, other: false }),
            step('near', { tag: true, other: false }),
            step('far', { tag: false, other: false }),
            step('near', { tag: true, other: true })
        ];
        const ranked = rank(steps, []);
        expect(ranked[0].streak).toBe(4);
        expect(winner(ranked)?.key).toBe('tag');
    });

    it('ignores devices that are not visible', () => {
        const s = newStep('near', 0);
        record(s, [dev('gone', true, { vis: false })]);
        expect(s.close.size).toBe(0);
    });

    it('keeps rows that share an id apart, by MAC', () => {
        const mine = (c: boolean) => dev('apple:1005:9-26', c, { mac: 'aa', name: 'Mine' });
        const theirs = dev('apple:1005:9-26', true, { mac: 'bb', name: 'Theirs' });
        const s = (phase: Phase, c: boolean) => {
            const st = newStep(phase, 0);
            record(st, [mine(c), theirs]);
            return st;
        };
        expect(winner(rank([s('near', true), s('far', false), s('near', true)], []))?.device.name).toBe('Mine');
    });

    it('still lists a device that dropped out of the latest poll, as away', () => {
        const steps = [step('near', { keys: true }), step('far', {}), step('near', { keys: true })];
        expect(winner(rank(steps, []))?.key).toBe('keys');
    });
});

describe('alias', () => {
    it('suggests type:kebab without repeating the type', () => {
        expect(suggestAlias("Darrell's Keys", 'keys')).toBe('keys:darrells');
        expect(suggestAlias('Kitchen Tile')).toBe('kitchen-tile');
    });

    it('flags IDs that will not stick', () => {
        expect(unstableId('4a1b2c3d4e5f')).toBe(true);
        expect(unstableId('apple:1005:9-26')).toBe(true);
        expect(unstableId('tile:c1d2e3f4a5b6')).toBe(false);
        expect(unstableId('iBeacon:e2c56db5-dffb-48d2-b060-d0f5a71096e0-1-2')).toBe(false);
    });
});

describe('fuzzy', () => {
    it('prefers substrings, then subsequences, and rejects the rest', () => {
        expect(fuzzy('fing', 'Fingerprints')).toBe(0);
        expect(fuzzy('fpr', 'Fingerprints')!).toBeGreaterThanOrEqual(100);
        expect(fuzzy('xyz', 'Fingerprints')).toBeUndefined();
        expect(fuzzy('', 'anything')).toBe(0);
        expect(fuzzy('da', 'Move it near and away')).toBeUndefined();
        expect(fuzzy('tv', `${'x'.repeat(150)} tv`)).toBeLessThan(100);
        expect(fuzzy('a b', 'axxb')).toBeUndefined();
    });
});
