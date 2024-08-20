import { readable } from 'svelte/store';
import { writable } from 'svelte/store';

export const extras = writable([], function start(set) {
    fetch("/extras")
        .then(d => d.json())
        .then(r => {
            set(r);
        })
        .catch((ex) => {
            set(null);
            console.log(ex);
        });
});

export const configs = readable([], function start(set) {
    var errors = 0;
    var outstanding = false;
    const interval = setInterval(() => {
        if (outstanding) return;
        outstanding = true;
        fetch("/json/configs")
            .then(d => d.json())
            .then(r => {
                outstanding = false;
                errors = 0;
                set(r);
            })
            .catch((ex) => {
                outstanding = false;
                if (errors > 5) set(null);
                console.log(ex);
            });
    }, 1000)

    return function stop() {
        clearInterval(interval);
    };
});

export const devices = readable([], function start(set) {
    var errors = 0;
    var outstanding = false;
    const interval = setInterval(() => {
        if (outstanding) return;
        outstanding = true;
        fetch("/json/devices?showAll")
            .then(d => d.json())
            .then(r => {
                outstanding = false;
                errors = 0;
                set(r);
            })
            .catch((ex) => {
                outstanding = false;
                if (errors > 5) set(null);
                console.log(ex);
            });
    }, 1000)

    return function stop() {
        clearInterval(interval);
    };
});

var initialValue = {};
var socket = null;
export const events = readable(initialValue, function start(set) {
    socket = new WebSocket(`${location.origin.replace('http://', 'ws://').replace('https://', 'wss://')}/ws`);
    socket.addEventListener('message', function (event) {
        initialValue = JSON.parse(event.data);
        console.log("Receive: " + event.data);
        set(initialValue);
    });

    return function stop() {
        socket.close();
    };
});

export function enroll(id, name) {
    var d = JSON.stringify({ command: "enroll", payload: id + "|" + name });
    console.log("Send: " + d);
    socket.send(d);
}

export function cancelEnroll() {
    var d = JSON.stringify({ command: "cancelEnroll" });
    console.log("Send: " + d);
    socket.send(d);
}
