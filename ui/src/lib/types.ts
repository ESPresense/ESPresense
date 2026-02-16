export interface Device {
    close: any;
    vis?: boolean;
    distance?: number;
    var?: number;
    mac?: string;
    rssi?: number;
    int?: number;
    id?: string;
    name?: string;
    "rssi@1m"?: number;
}

export interface Config {
    alias?: string;
    id?: string;
    name?: string;
    "rssi@1m"?: number;
}

export interface Events {
    room?: string;
    state?: {
        enrolling?: boolean;
        enrolledId?: string;
        remain?: number;
    };
}

export interface Configs {
    room: string;
    configs: Config[];
}

export interface Devices {
    room: string;
    devices: Device[];
}

export interface LetterMap {
    [key: string]: {
        name: string;
        value: string;
    };
}

export interface ExtraSettings {
    values: {
        [key: string]: any;
    };
    defaults: {
        [key: string]: any;
    };
}

export interface WebSocketCommand {
    command: string;
    payload?: string;
}

export type StartFunction<T> = (set: (value: T) => void) => (() => void) | void;

export interface MainSettings {
    values: {
        room: string;
        wifi_timeout: number;
        portal_timeout: number;
        "wifi-ssid": string;
        "wifi-password": string;
        static_ip: string;
        static_gw: string;
        static_subnet: string;
        static_dns: string;
        eth: string;
        mqtt_host: string;
        mqtt_port: number;
        mqtt_user: string;
        mqtt_pass: string;
        discovery: boolean;
        discovery_prefix: string;
        pub_tele: boolean;
        pub_rooms_dep: boolean;
        pub_devices: boolean;
        auto_update: boolean;
        prerelease: boolean;
        arduino_ota: boolean;
        update: string;
    };
    defaults: {
        room: string;
        "wifi-ssid": string;
        "wifi-password": string;
        wifi_timeout: number;
        portal_timeout: number;
        static_ip: string;
        static_gw: string;
        static_subnet: string;
        static_dns: string;
        eth: string;
        mqtt_host: string;
        mqtt_port: number;
        discovery: boolean;
        discovery_prefix: string;
        pub_tele: boolean;
        pub_rooms_dep: boolean;
        pub_devices: boolean;
        auto_update: boolean;
        prerelease: boolean;
        arduino_ota: boolean;
    };
}
