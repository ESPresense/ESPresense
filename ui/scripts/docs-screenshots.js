#!/usr/bin/env node
/**
 * Capture the web-UI screenshots used by the docs site (ESPresense.com, public/images/).
 *
 *   npm run screenshots
 *
 * Starts `vite dev` unless one is already running on BASE. Every endpoint a captured page reads is
 * mocked, so no node is needed and a bench node's leftover configuration never ends up in the docs.
 * Writes to ../../ESPresense.com/public/images (the docs repo as a sibling of this one); set
 * ESPRESENSE_DOCS_IMAGES to write somewhere else.
 */
import { spawn } from 'node:child_process';
import fs from 'node:fs';
import path from 'node:path';
import { fileURLToPath } from 'node:url';
import { chromium } from '@playwright/test';

const UI_DIR = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const BASE = process.env.ESPRESENSE_UI_URL ?? 'http://localhost:5199';
const OUT = path.resolve(process.env.ESPRESENSE_DOCS_IMAGES ?? path.join(UI_DIR, '../../ESPresense.com/public/images'));
// Phone width at 2x: what the docs show, and how the device UI is mostly used.
const VIEWPORT = { width: 430, height: 900 };

// Representative hardware settings for the docs: a node with a couple of sensors wired up. I2C
// addresses are left unset so the page shows their placeholder hints.
const hardware = {
	values: {
		leds: [{ type: 'pwm', pin: 2, control: 'status' }],
		inputs: [{ name: 'Hallway PIR', role: 'motion', pin: 4, type: 'pullup', timeout: 5 }],
		outputs: [],
		board: 'Espressif ESP32-DevKitC',
		dht11_pin: '-1',
		dht22_pin: '15',
		dhtTemp_offset: '0',
		dhtHumidity_offset: '0',
		I2C_Bus_1_SDA: '21',
		I2C_Bus_1_SCL: '22',
		I2C_Bus_2_SDA: '-1',
		I2C_Bus_2_SCL: '-1',
		I2CDebug: false,
		AHTX0_I2c_Bus: '1',
		BH1750_I2c_Bus: '1',
		BME280_I2c_Bus: '1',
		BMP180_I2c_Bus: '1',
		BMP280_I2c_Bus: '1',
		SHT_I2c_Bus: '1',
		TSL2561_I2c_Bus: '1',
		TSL2561_I2c_Gain: 'auto',
		SGP30_I2c_Bus: '1',
		SCD4x_I2c_Bus: '1',
		HX711_sckPin: '-1',
		HX711_doutPin: '-1',
		ds18b20_pin: '-1',
		dsTemp_offset: '0',
		batt_pin: '-1',
		batt_mult: '1',
		batt_type: '0',
		batt_axp192: false
	},
	defaults: {
		leds: [{ type: 'pwm', pin: 2, count: 1, control: 'status' }],
		inputs: [],
		outputs: [],
		I2C_Bus_1_SDA: '21',
		I2C_Bus_1_SCL: '22',
		dht11_pin: '-1',
		dht22_pin: '-1',
		batt_pin: '-1',
		batt_mult: '1',
		batt_type: '0',
		AHTX0_I2c_Bus: '1',
		BH1750_I2c_Bus: '1',
		BME280_I2c_Bus: '1',
		BMP180_I2c_Bus: '1',
		BMP280_I2c_Bus: '1',
		SHT_I2c_Bus: '1',
		TSL2561_I2c_Bus: '1',
		TSL2561_I2c_Gain: 'auto',
		SGP30_I2c_Bus: '1',
		SCD4x_I2c_Bus: '1',
		HX711_sckPin: '-1',
		HX711_doutPin: '-1'
	},
	// A plain ESP32's usable GPIOs (no flash/PSRAM pins); input-only ones are listed separately.
	pins: {
		usable: [2, 4, 5, 12, 13, 14, 15, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33, 34, 35, 36, 39],
		inputOnly: [34, 35, 36, 39]
	}
};

// GPIOs a typical ESP32 node reports as usable.
const pins = {
	usable: [2, 4, 5, 12, 13, 14, 15, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33, 34, 35, 36, 39],
	inputOnly: [34, 35, 36, 39]
};

// Representative network settings: a node on a LAN with MQTT reported to Home Assistant.
const network = {
	values: {
		room: 'office',
		'wifi-ssid': 'MyNetwork',
		'wifi-password': '********',
		eth: '0',
		discovery: true,
		pub_tele: true,
		pub_devices: true,
		auto_update: false,
		prerelease: false,
		arduino_ota: false
	},
	defaults: {
		room: 'node',
		wifi_timeout: 60,
		portal_timeout: 300,
		eth: '0',
		mqtt_host: 'homeassistant.local',
		mqtt_port: 1883,
		discovery: true,
		discovery_prefix: 'homeassistant',
		pub_tele: true,
		pub_devices: true,
		auto_update: false,
		prerelease: false,
		arduino_ota: false
	},
	pins
};

// What the node's WiFi scan sees: SSID -> RSSI, the node's own network strongest.
const wifiNetworks = {
	MyNetwork: -48,
	'MyNetwork-IoT': -51,
	'MyNetwork-Guest': -63,
	NETGEAR57: -74,
	'DIRECT-7B-HP OfficeJet': -81
};

/**
 * One entry per docs screenshot. `feed` maps a URL pattern to the JSON the page should see;
 * /json (page chrome) is always mocked and the websocket is refused.
 */
const SHOTS = [
	{ route: '/hardware', file: 'hardware_screen.png', feed: { '**/wifi/hardware*': hardware } },
	{
		route: '/network',
		file: 'network_screen.png',
		feed: { '**/wifi/main*': network, '**/wifi/scan*': { networks: wifiNetworks } }
	}
];

async function waitForServer(url, timeoutMs = 90_000) {
	const deadline = Date.now() + timeoutMs;
	while (Date.now() < deadline) {
		try {
			const res = await fetch(url, { signal: AbortSignal.timeout(2000) });
			if (res.ok) return true;
		} catch {
			/* not up yet */
		}
		await new Promise((r) => setTimeout(r, 500));
	}
	return false;
}

async function ensureDevServer() {
	if (await waitForServer(BASE, 1500)) return null;
	// Run vite itself rather than through npx, so kill() stops the server and not just a wrapper.
	const child = spawn(path.join(UI_DIR, 'node_modules/.bin/vite'), ['dev', '--port', new URL(BASE).port, '--strictPort'], {
		cwd: UI_DIR,
		stdio: 'inherit'
	});
	if (!(await waitForServer(BASE))) {
		child.kill();
		throw new Error(`vite dev did not start on ${BASE} within 90s`);
	}
	return child;
}

/** Route the endpoints a page reads to mock data, so the capture is stable and node-agnostic. */
async function mockDevice(page, feed = {}) {
	for (const [pattern, body] of Object.entries(feed)) {
		await page.route(pattern, (route) =>
			route.fulfill({ json: body, headers: { 'access-control-allow-origin': '*' } })
		);
	}
	await page.route('**/json*', (route) =>
		route.fulfill({ json: { room: 'office', firm: 'esp32', version: 'v4.0' } })
	);
	await page.route('**/ws*', (route) => route.abort());
}

/**
 * The app scrolls inside <main>, not the document, so `fullPage` only ever sees one viewport.
 * Grow the viewport to the content instead: that keeps the phone layout while capturing all of it.
 */
async function shoot(page, route, file) {
	// Reset first: a previous shot left the viewport as tall as its content, which would make
	// scrollHeight report that height instead of this page's.
	await page.setViewportSize(VIEWPORT);
	await page.goto(`${BASE}${route}`, { waitUntil: 'networkidle' });
	await page.waitForSelector('main', { timeout: 15_000 }).catch(() => {});
	// Let fonts settle and any enter transitions finish before the camera clicks.
	await page.evaluate(() => document.fonts.ready);
	await page.waitForTimeout(400);

	// The app scrolls inside <main>; the document itself is only ever viewport-tall.
	const contentHeight = await page.evaluate(() => {
		const el = document.querySelector('main') ?? document.body;
		return el.scrollHeight;
	});
	const viewport = page.viewportSize();
	await page.setViewportSize({ width: viewport.width, height: Math.ceil(contentHeight) });
	await page.waitForTimeout(200);

	const target = path.join(OUT, file);
	await page.screenshot({ path: target, fullPage: true });
	console.log(`${file}  ${viewport.width}x${Math.ceil(contentHeight)}`);
}

(async () => {
	if (!fs.existsSync(OUT)) throw new Error(`${OUT} does not exist. Set ESPRESENSE_DOCS_IMAGES to the docs repo's public/images.`);
	const server = await ensureDevServer();
	const browser = await chromium.launch();
	try {
		const context = await browser.newContext({
			viewport: VIEWPORT,
			deviceScaleFactor: 2,
			colorScheme: 'light'
		});
		const page = await context.newPage();
		for (const { route, file, feed } of SHOTS) {
			await mockDevice(page, feed);
			await shoot(page, route, file);
			await page.unrouteAll({ behavior: 'ignoreErrors' });
		}
	} finally {
		await browser.close();
		server?.kill();
	}
})().catch((err) => {
	console.error(err.message);
	process.exit(1);
});
