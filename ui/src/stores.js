import { readable } from 'svelte/store';

export const devices = readable([], function start(set) {
	var errors = 0;
	const interval = setInterval(() => {
		fetch(`/json/devices`)
			.then(d => d.json())
			.then(r => {
				errors = 0;
				set(r);
			})
			.catch((ex) => {
				if (errors > 5) set(null);
				console.log(ex);
			});
	}, 1000)

	return function stop() {
		clearInterval(interval);
	};
});
