// Alias suggestions shared by the Devices enroll dialog and the palette's tag step.

export const deviceTypes = ['watch', 'wallet', 'ipad', 'phone', 'airpods', 'laptop', 'node', 'keys', 'therm', 'flora', 'tile'];

export function kebab(name: string, type = ''): string {
    return name
        .toLowerCase()
        .split(/\s+/)
        .filter((w) => w !== type.toLowerCase())
        .join('-')
        .replace(/[']/g, '')
        .replace(/[^a-z0-9-]+/g, '-')
        .replace(/(^-|-$)/g, '');
}

export function suggestAlias(name: string, type = ''): string {
    const slug = kebab(name, type);
    return type && slug ? `${type}:${slug}` : slug;
}

/**
 * IDs that won't stick: a bare MAC (often a rotating random address) or Apple's shared
 * "apple:" model fingerprints. Phones and watches need pairing (Enroll) to get an IRK.
 */
export function unstableId(id: string): boolean {
    return /^[0-9a-f]{12}$/i.test(id) || id.startsWith('apple:');
}
