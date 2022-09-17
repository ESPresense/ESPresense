import { readable } from 'svelte/store';
import { writable } from 'svelte/store';

export const devices = readable([], function start(set) {
	var errors = 0;
	var outstanding = false;
	const interval = setInterval(() => {
		if (outstanding) return;
		outstanding = true;
		fetch(`/json/devices`)
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
	socket = new WebSocket(`${location.origin.replace('http://','ws://')}/ws`);

	socket.addEventListener('open', function (event) {
		console.log("It's open");
	});

	socket.addEventListener('message', function (event) {
		initialValue = JSON.parse(event.data);
		console.log("Receive: " + event.data);
		set(initialValue);
	});

	return function stop() {
		socket.close();
	};
});

export function slugify(dirty) {
    return dirty
        .toString()
        .normalize('NFD')                   // split an accented letter in the base letter and the acent
        .replace(/[\u0300-\u036f]/g, '')	// remove all previously split accents
        .toLowerCase()
        .trim()
        .replace(/[^a-z0-9 ]/g, '')			// remove all chars not letters, numbers and spaces (to be replaced)
        .replace(/\s+/g, "-");				// separator
};

export function enroll(name) {
	name = slugify(name);
	var d = JSON.stringify({ command:"enroll", payload:name });
	console.log("Send: " + d);
	socket.send(d);
}
