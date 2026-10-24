import { describe, it, expect } from 'vitest';
import { formParams } from './settings.js';

describe('formParams', () => {
	it('sends unchecked checkboxes as "0" so the device turns them off', () => {
		const form = document.createElement('form');
		form.innerHTML = `
			<input name="room" value="office">
			<input type="checkbox" name="discovery" value="1" checked>
			<input type="checkbox" name="pub_tele" value="1">
			<input type="checkbox" name="axp192">
			<input type="checkbox" name="locked" disabled>`;
		expect(formParams(form).toString()).toBe('room=office&discovery=1&pub_tele=0&axp192=0');
	});
});
