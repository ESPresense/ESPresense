import { readable, writable } from 'svelte/store';
import type { Extras, Configs, Devices, WebSocketCommand, StartFunction } from './types';

// Dark mode store with persistence
export const darkMode = writable<boolean>(false, (set) => {
    if (typeof window !== 'undefined') {
        // Initialize from localStorage or system preference
        const stored = localStorage.getItem('darkMode');
        const systemPrefersDark = window.matchMedia('(prefers-color-scheme: dark)').matches;
        const isDark = stored ? stored === 'true' : systemPrefersDark;
        set(isDark);

        // Update document class
        if (isDark) document.documentElement.classList.add('dark');
    }

    return () => {};
});

// Subscribe to changes and update localStorage and document class
if (typeof window !== 'undefined') {
    darkMode.subscribe(isDark => {
        localStorage.setItem('darkMode', isDark.toString());
        if (isDark) {
            document.documentElement.classList.add('dark');
        } else {
            document.documentElement.classList.remove('dark');
        }
    });
}

// Export the stores with their types
export const extras = writable<Extras | null>({}, function start(set) {
    fetch("/extras")
        .then(d => d.json())
        .then((r: Extras) => {
            set(r);
        })
        .catch((ex) => {
            set(null);
            console.log(ex);
        });
});

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
                console.log(ex);
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
                console.log(ex);
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
