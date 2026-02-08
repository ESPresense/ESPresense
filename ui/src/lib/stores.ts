import { readable, writable } from 'svelte/store';
import type { ExtraSettings, Configs, Devices, WebSocketCommand, StartFunction, MainSettings } from './types';

// Room name store that stops polling once room name is found
export const roomName = readable<string>('', function start(set) {
    let errors = 0;
    let outstanding = false;
    const interval = setInterval(() => {
        if (outstanding) return;
        outstanding = true;
        fetch("/json")
            .then(d => d.json())
            .then((r: { room: string }) => {
                outstanding = false;
                errors = 0;
                if (r.room) {
                    clearInterval(interval); // Stop polling once we have the room name
                    set(r.room);
                }
            })
            .catch((ex) => {
                outstanding = false;
                if (errors++ > 5) set('');
                console.error('Failed to fetch room name:', ex);
            });
    }, 1000);

    return function stop() {
        clearInterval(interval);
    };
});

type ThemeMode = 'system' | 'light' | 'dark';

const themeStorageKey = 'theme';
const legacyDarkModeKey = 'darkMode';

function getSystemPrefersDark(): boolean {
    return typeof window !== 'undefined' && typeof window.matchMedia === 'function'
        ? window.matchMedia('(prefers-color-scheme: dark)').matches
        : false;
}

function applyTheme(mode: ThemeMode): void {
    if (typeof document === 'undefined') return;
    const isDark = mode === 'dark' || (mode === 'system' && getSystemPrefersDark());
    document.documentElement.classList.toggle('dark', isDark);
}

function resolveInitialTheme(): ThemeMode {
    if (typeof window === 'undefined') return 'system';
    const storedTheme = localStorage.getItem(themeStorageKey);
    if (storedTheme === 'light' || storedTheme === 'dark' || storedTheme === 'system') {
        return storedTheme;
    }
    const legacyDarkMode = localStorage.getItem(legacyDarkModeKey);
    if (legacyDarkMode === 'true' || legacyDarkMode === 'false') {
        const migrated = legacyDarkMode === 'true' ? 'dark' : 'light';
        localStorage.setItem(themeStorageKey, migrated);
        return migrated;
    }
    return 'system';
}

// Theme mode store with persistence
export const themeMode = writable<ThemeMode>('system', (set) => {
    if (typeof window !== 'undefined') {
        const initialTheme = resolveInitialTheme();
        set(initialTheme);
        applyTheme(initialTheme);
    }

    return () => {};
});

// Subscribe to changes and update localStorage and document class
if (typeof window !== 'undefined') {
    themeMode.subscribe((mode) => {
        localStorage.setItem(themeStorageKey, mode);
        applyTheme(mode);
    });
}

export const configs = readable<Configs | null>({ room: '', configs: [] }, function start(set) {
    let errors = 0;
    let outstanding = false;
    const interval = setInterval(() => {
        if (outstanding) return;
        outstanding = true;
        fetch("/json/configs")
            .then(d => d.json())
            .then((r: Configs) => {
                outstanding = false;
                errors = 0;
                set(r);
            })
            .catch((ex) => {
                outstanding = false;
                if (errors++ > 5) set(null);
                console.error('Failed to fetch configs:', ex);
            });
    }, 1000);

    return function stop() {
        clearInterval(interval);
    };
});

export const devices = readable<Devices | null>({ room: '', devices: [] }, function start(set) {
    let errors = 0;
    let outstanding = false;
    const interval = setInterval(() => {
        if (outstanding) return;
        outstanding = true;
        fetch("/json/devices?showAll")
            .then(d => d.json())
            .then((r: Devices) => {
                outstanding = false;
                errors = 0;
                set(r);
            })
            .catch((ex) => {
                outstanding = false;
                if (errors++ > 5) set(null);
                console.error('Failed to fetch devices:', ex);
            });
    }, 1000);

    return function stop() {
        clearInterval(interval);
    };
});

const initialValue: any = {};
let socket: WebSocket | null = null;

export const events = readable<any>(initialValue, function start(set) {
    socket = new WebSocket(`${location.origin.replace('http://', 'ws://').replace('https://', 'wss://')}/ws`);
    socket.addEventListener('message', function (event: MessageEvent) {
        const parsedData = JSON.parse(event.data);
        console.log("Receive: " + event.data);
        set(parsedData);
    });

    return function stop() {
        socket?.close();
    };
});

export function enroll(id: string, name: string): void {
    const command: WebSocketCommand = {
        command: "enroll",
        payload: `${id}|${name}`
    };
    const data = JSON.stringify(command);
    console.log("Send: " + data);
    socket?.send(data);
}

export function cancelEnroll(): void {
    const command: WebSocketCommand = {
        command: "cancelEnroll"
    };
    const data = JSON.stringify(command);
    console.log("Send: " + data);
    socket?.send(data);
}

export const mobileMenuOpen = writable<boolean>(false);

export const extraSettings = writable<ExtraSettings | null>(null, function start(set) {
    fetch("/wifi/extras")
        .then(d => d.json())
        .then((r: ExtraSettings) => {
            set(r);
        })
        .catch((ex) => {
            set(null);
            console.error('Failed to fetch extra settings:', ex);
        });
});

export const hardwareSettings = writable<ExtraSettings | null>(null, function start(set) {
    fetch("/wifi/hardware")
        .then(d => d.json())
        .then((r: ExtraSettings) => {
            set(r);
        })
        .catch((ex) => {
            set(null);
            console.error('Failed to fetch hardware settings:', ex);
        });
});

export const mainSettings = writable<MainSettings | null>(null, function start(set) {
    fetch("/wifi/main")
        .then(d => d.json())
        .then((r: MainSettings) => {
            set(r);
        })
        .catch((ex) => {
            set(null);
            console.error('Failed to fetch main settings:', ex);
        });
});
