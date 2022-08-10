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
export const events = readable(initialValue, function start(set) {
	const socket = new WebSocket(`${location.origin.replace('http://','ws://')}/ws`);

	socket.addEventListener('open', function (event) {
		console.log("It's open");
	});

	socket.addEventListener('message', function (event) {
		initialValue = JSON.parse(event.data);
		console.log(event.data);
		set(initialValue);
	});

	return function stop() {
		socket.close();
	};
});

