import { describe, it, expect } from 'vitest';
import { newStep, record, rank, winner, type Phase, type Step } from './find.js';
import { suggestAlias, unstableId } from './alias.js';
import { fuzzy } from './search.js';
import type { Device } from '../types.js';

const dev = (id: string, rssi: number, extra: Partial<Device> = {}): Device => ({ id, rssi, vis: true, close: false, ...extra });

/** one step of polls: every device at a fixed level for `polls` readings */
function step(phase: Phase, levels: Record<string, number>, polls = 5): Step {
    const s = newStep(phase, 0);
    for (let i = 0; i < polls; i++) record(s, Object.entries(levels).map(([id, r]) => dev(id, r)));
    return s;
}

describe('find', () => {
    it('picks the one device that follows near/far/near', () => {
        const steps = [
            step('near', { keys: -45, tv: -70, fridge: -80 }),
            step('far', { keys: -78, tv: -71, fridge: -79 }),
            step('near', { keys: -47, tv: -69, fridge: -81 })
        ];
        const devices = [dev('keys', -47), dev('tv', -69), dev('fridge', -81)];
        const ranked = rank(steps, devices);
        expect(ranked[0].key).toBe('keys');
        expect(ranked[0].consistent).toBe(true);
        expect(ranked[0].score).toBeCloseTo(32);
        expect(winner(ranked, 2)?.key).toBe('keys');
    });

    it('waits for two boundaries before naming a winner', () => {
        const steps = [step('near', { keys: -45, tv: -70 }), step('far', { keys: -80, tv: -70 })];
        const ranked = rank(steps, [dev('keys', -80), dev('tv', -70)]);
        expect(ranked[0].key).toBe('keys');
        expect(winner(ranked, 1)).toBeUndefined();
    });

    it("won't choose between two devices that both followed every move", () => {
        // Keys on the same ring as a tag: both move together.
        const steps = [
            step('near', { keys: -45, tag: -50 }),
            step('far', { keys: -80, tag: -82 }),
            step('near', { keys: -46, tag: -52 })
        ];
        expect(winner(rank(steps, [dev('keys', -46), dev('tag', -52)]), 2)).toBeUndefined();
    });

    it('lets a consistent weaker device veto the winner', () => {
        const steps = [
            step('near', { keys: -45, weak: -60 }),
            step('far', { keys: -80, weak: -63 }),
            step('near', { keys: -46, weak: -60 })
        ];
        expect(winner(rank(steps, [dev('keys', -46), dev('weak', -60)]), 2)).toBeUndefined();
    });

    it('counts only the settled tail of a step, not the window catching up', () => {
        const near = newStep('near', 0);
        for (const r of [-80, -80, -70, -60, -50, -46, -45, -45]) record(near, [dev('keys', r)]);
        const far = step('far', { keys: -80 });
        const [c] = rank([near, far], [dev('keys', -80)]);
        expect(c.score).toBeCloseTo(35);
    });

    it('ignores devices that are not visible or have no RSSI', () => {
        const s = newStep('near', 0);
        record(s, [dev('gone', -40, { vis: false }), { id: 'quiet', vis: true, close: false }]);
        expect(s.readings.size).toBe(0);
    });

    it('keeps rows that share an id apart, by MAC', () => {
        const mover = (r: number) => dev('apple:1005:9-26', r, { mac: 'aa', name: 'Mine' });
        const still = dev('apple:1005:9-26', -60, { mac: 'bb', name: 'Theirs' });
        const s = (phase: Phase, r: number) => {
            const st = newStep(phase, 0);
            for (let i = 0; i < 4; i++) record(st, [mover(r), still]);
            return st;
        };
        const ranked = rank([s('near', -45), s('far', -80), s('near', -45)], [mover(-45), still]);
        expect(winner(ranked, 2)?.device.name).toBe('Mine');
    });

    it('still ranks a device that dropped out of the latest poll', () => {
        const steps = [step('near', { keys: -45, tv: -70 }), step('far', { keys: -80, tv: -70 }), step('near', { keys: -45, tv: -70 })];
        expect(winner(rank(steps, [dev('tv', -70)]), 2)?.key).toBe('keys');
    });

    it('gives no level to a step with fewer than three readings', () => {
        const steps = [step('near', { blip: -45 }, 1), step('far', { blip: -80 }, 1), step('near', { blip: -45 }, 1)];
        expect(rank(steps, [dev('blip', -45)])).toEqual([]);
    });

    it('marks a device that moved the wrong way as inconsistent', () => {
        const steps = [
            step('near', { a: -45 }),
            step('far', { a: -80 }),
            step('near', { a: -82 })
        ];
        const [c] = rank(steps, [dev('a', -82)]);
        expect(c.consistent).toBe(false);
        expect(winner([c], 2)).toBeUndefined();
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
