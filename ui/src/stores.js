import { readable } from 'svelte/store';

export const devices = readable([], function start(set) {
	const interval = setInterval(() => {
		fetch(`/json/devices`)
			.then(d => d.json())
			.then(r => set(r.devices))
			.catch((ex) => { set(null); console.log(ex); });
	}, 1000)

	return function stop() {
		clearInterval(interval);
	};
});
