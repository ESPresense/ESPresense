/**
 * Case-insensitive match; lower is better, undefined is no match. A substring scores its
 * position. From three characters a scattered subsequence also matches, scoring 100 plus
 * the gaps; shorter queries would match nearly everything that way.
 */
export function fuzzy(query: string, text: string): number | undefined {
    const q = query.toLowerCase().trim();
    if (!q) return 0;
    const t = text.toLowerCase();
    const at = t.indexOf(q);
    if (at >= 0) return Math.min(at, 99);
    if (q.length < 3) return undefined;
    let ti = 0;
    let gaps = 0;
    for (const ch of q) {
        if (ch === ' ') continue;
        const found = t.indexOf(ch, ti);
        if (found < 0) return undefined;
        gaps += found - ti;
        ti = found + 1;
    }
    return 100 + gaps;
}
