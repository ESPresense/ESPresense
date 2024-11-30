export interface Device {
    vis?: boolean;
    distance?: number;
    var?: number;
    mac?: string;
    rssi?: number;
    int?: number;
}

export interface Config {
    alias?: string;
    id?: string;
    name?: string;
    "rssi@1m"?: number;
    close?: boolean;
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

export interface TableColumn<T> {
    key: string;
    title: string;
    value: (v: T) => any;
    renderValue?: (v: T) => string;
    sortable?: boolean;
    filterOptions?: (rows: T[]) => Array<{ name: string; value: string | boolean }>;
    filterValue?: (v: T) => string;
    headerClass?: string;
    class?: string;
}

export interface Extras {
    [key: string]: any;
}

export interface WebSocketCommand {
    command: string;
    payload?: string;
}

export type StartFunction<T> = (set: (value: T) => void) => (() => void) | void;
