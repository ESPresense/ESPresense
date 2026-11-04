<script lang="ts">
    import { goto } from "$app/navigation";
    import { resolve } from "$app/paths";
    import { tick } from "svelte";
    import Icon from "./Icon.svelte";
    import { configs, devices, findConfig, palette, roomName, tagFingerprint as tagSeen, type PaletteState } from "#lib/stores.js";
    import type { Config, Device } from "#lib/types.js";
    import { newStep, rank, record, winner, rowKey, type Candidate, type Step } from "#lib/utils/find.js";
    import { deviceTypes, suggestAlias, unstableId } from "#lib/utils/alias.js";
    import { fuzzy } from "#lib/utils/search.js";


    type Item = {
        key: string;
        group: string;
        icon: string;
        title: string;
        detail?: string;
        keywords?: string;
        run: () => void;
    };

    let query = $state("");
    // The cursor follows a row's key, not its index: the lists refresh every poll.
    let activeKey = $state<string>();
    let input = $state<HTMLInputElement>();
    let list = $state<HTMLElement>();

    const pages: Array<{ path: "/network" | "/settings" | "/hardware" | "/templates" | "/devices" | "/fingerprints"; title: string; icon: string; keywords: string }> = [
        { path: "/network", title: "Network", icon: "network", keywords: "wifi ethernet mqtt ntp room" },
        { path: "/settings", title: "Settings", icon: "contact", keywords: "scanning counting filtering calibration" },
        { path: "/hardware", title: "Hardware", icon: "hardware", keywords: "gpio pins led sensors battery" },
        { path: "/templates", title: "Templates", icon: "template", keywords: "import board" },
        { path: "/devices", title: "Devices", icon: "device", keywords: "configured aliases enroll pair" },
        { path: "/fingerprints", title: "Fingerprints", icon: "fingerprint", keywords: "ble seen nearby rssi" }
    ];

    // Poll configs while the palette is open (a store subscribes on first read): Find and the
    // name step both look up the fingerprint's existing config.
    $effect(() => {
        if ($palette) void $configs;
    });

    function close() {
        palette.set(null);
    }

    function show(next: PaletteState) {
        palette.set(next);
    }

    function tagFingerprint(d: Device) {
        tagSeen(d, $configs?.configs);
    }

    function tagConfig(c: Config) {
        if (!c.id) return;
        show({ view: "tag", id: c.id, name: c.name, alias: c.alias, rssi1m: c["rssi@1m"] });
    }

    let items = $derived.by((): Item[] => {
        const all: Item[] = [
            {
                key: "find",
                group: "Actions",
                icon: "fingerprint",
                title: "Find a device…",
                detail: "Move it near the node and away to pick it out",
                keywords: "identify locate which tag name alias add",
                run: () => show({ view: "find" })
            },
            ...pages.map((p) => ({
                key: `page:${p.path}`,
                group: "Pages",
                icon: p.icon,
                title: p.title,
                keywords: p.keywords,
                run: () => {
                    close();
                    goto(resolve(p.path));
                }
            }))
        ];

        const seen = ($devices?.devices ?? []).filter((d) => d.vis && rowKey(d));
        seen.sort((a, b) => (a.distance ?? Infinity) - (b.distance ?? Infinity));
        for (const d of seen) {
            all.push({
                key: `fp:${rowKey(d)}`,
                group: "Fingerprints",
                icon: "fingerprint",
                title: d.name || d.id || d.mac!,
                detail: [d.name ? d.id : "", d.distance != null ? `${d.distance.toFixed(1)} m` : "", d.rssi != null ? `${d.rssi} dBm` : ""].filter(Boolean).join(" · "),
                keywords: `${d.id ?? ""} ${d.mac ?? ""}`,
                run: () => tagFingerprint(d)
            });
        }

        for (const c of $configs?.configs ?? []) {
            if (!c.id) continue;
            all.push({
                key: `cfg:${c.id}`,
                group: "Devices",
                icon: "device",
                title: c.name || c.alias || c.id,
                detail: [c.alias, c.id !== c.alias ? c.id : ""].filter(Boolean).join(" · "),
                keywords: `${c.alias ?? ""} ${c.id}`,
                run: () => tagConfig(c)
            });
        }

        if (!query.trim()) {
            // Nothing typed: actions, pages and the five closest fingerprints.
            let fp = 0;
            return all.filter((i) => i.group !== "Devices" && (i.group !== "Fingerprints" || fp++ < 5));
        }

        const scored = all
            .map((i) => ({ i, s: fuzzy(query, `${i.title} ${i.keywords ?? ""}`) }))
            .filter((x) => x.s !== undefined) as Array<{ i: Item; s: number }>;
        const order = ["Actions", "Pages", "Devices", "Fingerprints"];
        // Substring matches before loose ones, then by group so the groups stay together.
        const loose = (s: number) => Number(s >= 100);
        scored.sort((a, b) => loose(a.s) - loose(b.s) || order.indexOf(a.i.group) - order.indexOf(b.i.group) || a.s - b.s);
        // Keep each group short so a common letter doesn't list every fingerprint.
        const perGroup = new Map<string, number>();
        return scored
            .map((x) => x.i)
            .filter((i) => {
                const n = perGroup.get(i.group) ?? 0;
                perGroup.set(i.group, n + 1);
                return n < 8;
            });
    });

    $effect(() => {
        // A new query starts back at the top.
        void query;
        activeKey = undefined;
    });
    // ponytail: a row that drops out of the list puts the cursor back at the top
    let active = $derived(Math.max(0, items.findIndex((i) => i.key === activeKey)));

    function scrollActive() {
        tick().then(() => list?.querySelector('[aria-selected="true"]')?.scrollIntoView({ block: "nearest" }));
    }

    // ---- Find ----

    let steps = $state.raw<Step[]>([]);
    let found = $state<Candidate | undefined>();

    function startFind() {
        steps = [newStep("near", Date.now())];
        found = undefined;
    }

    let current = $derived(steps[steps.length - 1]);
    // Live, including the step being held: the list is the feedback.
    let ranked = $derived(rank(steps, $devices?.devices ?? []));
    let still = $derived(ranked.filter((c) => c.matched === steps.length).length);

    // Steps end on a click, never on a timer: the far step takes as long as the walk does.
    // Rounds go on until one device is left.
    function advance() {
        // ponytail: the 500 ms is only so a double click can't skip the step it just started
        if (!current || found || Date.now() - current.startedAt < 500) return;
        const done = steps;
        const w = winner(rank(done, $devices?.devices ?? []), done.length);
        if (w) {
            steps = [...done];
            found = w;
            return;
        }
        steps = [...done, newStep(current.phase === "near" ? "far" : "near", Date.now())];
    }

    $effect(() => {
        // Each poll of /json/devices goes into the step being held.
        const list = $devices?.devices;
        if ($palette?.view !== "find" || !list || !current || found) return;
        record(current, list);
    });

    $effect(() => {
        if ($palette?.view === "find" && steps.length === 0) startFind();
    });

    $effect(() => {
        // Leaving Find (or closing) throws its readings away.
        if ($palette?.view !== "find" && steps.length) {
            steps = [];
            found = undefined;
        }
    });

    // ---- Tag ----

    let tagType = $state("");
    let tagName = $state("");
    let tagAlias = $state("");
    // ids with a save in flight, so a pending save on one tag never blocks or clears another
    let saving = $state(new Set<string>());
    let saveError = $state("");
    let saved = $state(false);

    let tag = $derived($palette?.view === "tag" ? $palette : undefined);
    let tagKey = $derived(tag?.id);
    let savingTag = $derived(!!tag && saving.has(tag.id));

    $effect(() => {
        // A new fingerprint to tag: start from what's already configured for it.
        if (!tagKey) return;
        const t = $palette as Extract<PaletteState, { view: "tag" }>;
        tagType = "";
        tagName = t.name ?? "";
        tagAlias = t.alias && t.alias !== t.id ? t.alias : "";
        saveError = "";
        saved = false;
    });

    async function saveTag(e?: Event) {
        e?.preventDefault();
        if (!tag || !tagName.trim() || saving.has(tag.id)) return;
        const submitted = tag;
        saving = new Set(saving).add(submitted.id);
        saveError = "";
        // Look the config up again: the step may have opened before /json/configs first loaded,
        // and saving under the reported alias would make a second config (and the node deletes
        // the original). Carry the calibration along, since the node republishes the whole config.
        const cfg = findConfig(submitted.id, submitted.mac, $configs?.configs);
        const rssi1m = cfg?.["rssi@1m"] ?? submitted.rssi1m;
        const body: Record<string, unknown> = { id: cfg?.id ?? submitted.id, alias: tagAlias.trim() || suggestAlias(tagName, tagType), name: tagName.trim() };
        if (rssi1m != null) body["rssi@1m"] = rssi1m;
        let ok = false;
        try {
            const r = await fetch("/json/configs", {
                method: "POST",
                headers: { "Content-Type": "application/json" },
                body: JSON.stringify(body)
            });
            ok = r.ok;
        } catch {
            ok = false;
        }
        saving.delete(submitted.id);
        saving = new Set(saving);
        // The palette may have moved on to another tag while this was in flight.
        if ($palette !== submitted) return;
        if (ok) saved = true;
        else saveError = "The node didn't save it. Try again.";
    }

    // ---- keys and focus ----

    function onWindowKey(e: KeyboardEvent) {
        if ((e.metaKey || e.ctrlKey) && e.key.toLowerCase() === "k") {
            e.preventDefault();
            palette.update((p) => (p ? null : { view: "search" }));
            return;
        }
        if (!$palette) return;
        if (e.key === "Tab" && panel) {
            // Wrap Tab inside the dialog; the page behind it is inert.
            const els = [...panel.querySelectorAll<HTMLElement>("input, select, button, a[href], [tabindex]:not([tabindex='-1'])")].filter((el) => !(el as HTMLButtonElement).disabled);
            if (!els.length) return;
            const first = els[0];
            const last = els[els.length - 1];
            const inside = panel.contains(document.activeElement);
            if (e.shiftKey && (!inside || document.activeElement === first)) {
                e.preventDefault();
                last.focus();
            } else if (!e.shiftKey && (!inside || document.activeElement === last)) {
                e.preventDefault();
                first.focus();
            }
            return;
        }
        if (e.key === "Escape") {
            e.preventDefault();
            // Escape steps back to search before it closes.
            if ($palette.view === "search") close();
            else show({ view: "search" });
        }
    }

    function onSearchKey(e: KeyboardEvent) {
        if (e.key === "ArrowDown") {
            e.preventDefault();
            activeKey = items[Math.min(active + 1, items.length - 1)]?.key;
            scrollActive();
        } else if (e.key === "ArrowUp") {
            e.preventDefault();
            activeKey = items[Math.max(active - 1, 0)]?.key;
            scrollActive();
        } else if (e.key === "Enter") {
            e.preventDefault();
            items[active]?.run();
        }
    }

    function onFindKey(e: KeyboardEvent) {
        if (e.key !== "Enter" || (e.target as HTMLElement).tagName === "BUTTON") return;
        e.preventDefault();
        if (found) tagFingerprint(found.device);
        else advance();
    }

    let panel = $state<HTMLElement>();
    $effect(() => {
        const view = $palette?.view;
        if (!view) return;
        if (view === "search") query = "";
        tick().then(() => {
            if (view === "search") input?.focus();
            else (panel?.querySelector("[data-autofocus]") as HTMLElement | null)?.focus();
        });
    });

</script>

<svelte:window onkeydown={onWindowKey} />

{#if $palette}
    <div class="fixed inset-0 z-50 flex items-start justify-center p-4 pt-[10vh]" role="presentation">
        <button type="button" class="absolute inset-0 bg-gray-900/50 cursor-default" aria-label="Close" tabindex="-1" onclick={close}></button>
        <div
            bind:this={panel}
            class="relative w-full max-w-xl overflow-hidden rounded-xl bg-white shadow-2xl ring-1 ring-black/10 dark:bg-gray-800 dark:ring-white/10"
            role="dialog"
            aria-modal="true"
            aria-label={$palette.view === "find" ? "Find a device" : $palette.view === "tag" ? "Name this device" : "Search"}
        >
            {#if $palette.view === "search"}
                <div class="flex items-center gap-3 border-b border-gray-200 px-4 dark:border-gray-700">
                    <svg class="h-5 w-5 shrink-0 text-gray-400" viewBox="0 0 20 20" fill="currentColor" aria-hidden="true"><path fill-rule="evenodd" d="M9 3.5a5.5 5.5 0 1 0 0 11 5.5 5.5 0 0 0 0-11ZM2 9a7 7 0 1 1 12.45 4.39l3.08 3.08a.75.75 0 1 1-1.06 1.06l-3.08-3.08A7 7 0 0 1 2 9Z" clip-rule="evenodd" /></svg>
                    <input
                        bind:this={input}
                        bind:value={query}
                        onkeydown={onSearchKey}
                        class="h-12 w-full rounded-none border-0 bg-transparent px-0 py-0 text-gray-900 placeholder:text-gray-400 outline-none focus:ring-0 dark:bg-transparent dark:text-white"
                        placeholder="Search pages, devices, fingerprints…"
                        role="combobox"
                        aria-label="Search"
                        aria-expanded="true"
                        aria-controls="palette-results"
                        aria-activedescendant={items[active] ? `palette-${active}` : undefined}
                        autocomplete="off"
                        spellcheck="false"
                    />
                    <kbd class="hidden sm:block rounded border border-gray-300 px-1.5 text-xs text-gray-500 dark:border-gray-600 dark:text-gray-400">esc</kbd>
                </div>
                <ul bind:this={list} id="palette-results" role="listbox" class="max-h-[60vh] overflow-y-auto p-2">
                    {#each items as item, i (item.key)}
                        {#if i === 0 || items[i - 1].group !== item.group}
                            <li role="presentation" class="px-3 pb-1 pt-2 text-xs font-semibold uppercase tracking-wider text-gray-500 dark:text-gray-400">{item.group}</li>
                        {/if}
                        <!-- svelte-ignore a11y_click_events_have_key_events (the search box drives the keys) -->
                        <li
                            id={`palette-${i}`}
                            role="option"
                            aria-selected={i === active}
                            class="flex cursor-pointer items-center gap-3 rounded-lg px-3 py-2 {i === active ? 'bg-blue-600 text-white' : 'text-gray-800 dark:text-gray-200'}"
                            onpointermove={() => (activeKey = item.key)}
                            onclick={() => item.run()}
                        >
                            <Icon name={item.icon} class_="w-5 h-5 shrink-0" />
                            <span class="truncate">{item.title}</span>
                            {#if item.detail}
                                <span class="ml-auto truncate text-xs {i === active ? 'text-blue-100' : 'text-gray-500 dark:text-gray-400'}">{item.detail}</span>
                            {/if}
                        </li>
                    {:else}
                        <li class="px-3 py-6 text-center text-sm text-gray-500 dark:text-gray-400">Nothing matches “{query}”</li>
                    {/each}
                </ul>
            {:else if $palette.view === "find"}
                <!-- svelte-ignore a11y_no_static_element_interactions -->
                <div class="p-5" onkeydown={onFindKey}>
                    <div class="mb-4 flex items-baseline justify-between">
                        <h2 class="mb-0 text-lg font-semibold text-gray-900 dark:text-white">Find a device</h2>
                        <span class="text-xs text-gray-500 dark:text-gray-400">Step {steps.length}</span>
                    </div>

                    {#if found}
                        <div class="rounded-lg border border-green-300 bg-green-50 p-4 dark:border-green-700 dark:bg-green-900/30" data-testid="find-found">
                            <div class="text-sm font-medium text-green-800 dark:text-green-200">Found it</div>
                            <div class="mt-1 text-lg font-semibold text-gray-900 dark:text-white">{found.device.name || found.key}</div>
                            <div class="text-xs text-gray-600 dark:text-gray-300">{found.device.id ?? found.key} · close every time you brought it, away every time you took it</div>
                        </div>
                    {:else if current}
                        <div class="rounded-lg p-4 {current.phase === 'near' ? 'bg-blue-50 dark:bg-blue-900/30' : 'bg-gray-100 dark:bg-gray-700'}" data-testid="find-step" data-phase={current.phase}>
                            <div class="text-base font-semibold text-gray-900 dark:text-white">
                                {#if current.phase === "near"}Hold it right against {$roomName ? `the ${$roomName} node` : "this node"}{:else}Take it away, as far as you like{/if}
                            </div>
                            <div class="mt-1 text-sm text-gray-600 dark:text-gray-300">
                                {#if current.phase === "near"}Touching the node is ideal. The node averages signal over ~15 s, so wait until it shows as ● below, then click Next step.{:else}Another room is best. No rush: wait until it shows as ○ below, click Next step, then bring it back.{/if}
                            </div>
                            {#if steps.length >= 3}
                                <div class="mt-2 text-xs text-gray-600 dark:text-gray-300" data-testid="find-progress">
                                    {#if still === 0}Nothing has followed every move yet. Hold it right against the node, and take it further away.{:else}{still} still following every move. Another round narrows it down, or pick one below.{/if}
                                </div>
                            {/if}
                        </div>
                    {/if}

                    {#if ranked.length}
                        <div class="mt-4">
                            <div class="mb-1 flex justify-between text-xs font-semibold uppercase tracking-wider text-gray-500 dark:text-gray-400"><span>{steps.length > 1 ? "Followed your moves" : "Close to the node"}</span><span class="font-mono normal-case tracking-normal">● close · ○ away</span></div>
                            <ul class="divide-y divide-gray-100 dark:divide-gray-700" data-testid="find-candidates">
                                {#each ranked.slice(0, 5) as c (c.key)}
                                    <li>
                                        <button
                                            type="button"
                                            class="flex w-full items-center gap-3 rounded px-2 py-2 text-left hover:bg-gray-100 dark:hover:bg-gray-700 {found?.key === c.key ? 'bg-green-100 dark:bg-green-900/40' : ''}"
                                            onclick={() => tagFingerprint(c.device)}
                                        >
                                            <span class="shrink-0 font-mono text-sm {c.matched === steps.length ? 'text-green-700 dark:text-green-400' : 'text-gray-400'}">{c.close.map((x) => (x ? "●" : "○")).join(" ")}</span>
                                            <span class="truncate text-sm text-gray-900 dark:text-gray-100">{c.device.name || c.device.id || c.key}</span>
                                            <span class="ml-auto shrink-0 font-mono text-xs text-gray-500 dark:text-gray-400">{c.device.rssi != null ? `${c.device.rssi} dBm` : ""}</span>
                                        </button>
                                    </li>
                                {/each}
                            </ul>
                        </div>
                    {:else if !found}
                        <!-- Until there are candidates, so the panel doesn't shrink under a second click. -->
                        <p class="mt-4 text-sm text-gray-500 dark:text-gray-400">Whatever is close to the node shows up here once its ~15 s signal average catches up. You'll then take it away and bring it back, as many rounds as it takes.</p>
                    {/if}

                    <div class="mt-5 flex items-center justify-end gap-2">
                        <button type="button" class="rounded-md border border-gray-300 px-3 py-1.5 text-sm text-gray-700 hover:bg-gray-100 dark:border-gray-600 dark:text-gray-200 dark:hover:bg-gray-700" onclick={startFind}>Start over</button>
                        {#if found}
                            <button type="button" data-autofocus class="rounded-md bg-blue-600 px-3 py-1.5 text-sm font-medium text-white hover:bg-blue-700" onclick={() => tagFingerprint(found!.device)}>Name it</button>
                        {:else}
                            <button type="button" data-autofocus class="rounded-md bg-blue-600 px-3 py-1.5 text-sm font-medium text-white hover:bg-blue-700 disabled:opacity-50" onclick={advance}>Next step</button>
                        {/if}
                    </div>
                </div>
            {:else if tag}
                <form class="p-5" onsubmit={saveTag}>
                    <h2 class="mb-1 text-lg font-semibold text-gray-900 dark:text-white">{saved ? "Saved" : "Name this device"}</h2>
                    <p class="mb-4 break-all font-mono text-xs text-gray-500 dark:text-gray-400">{[tag.id, tag.mac !== tag.id ? tag.mac : "", tag.rssi != null ? `${tag.rssi} dBm` : ""].filter(Boolean).join(" · ")}</p>

                    {#if saved}
                        <p class="text-sm text-gray-700 dark:text-gray-300">
                            Nodes will now report <span class="font-semibold">{tagName}</span> as
                            <span class="font-mono">{tagAlias.trim() || suggestAlias(tagName, tagType)}</span>.
                        </p>
                        <div class="mt-5 flex justify-end gap-2">
                            <button type="button" class="rounded-md border border-gray-300 px-3 py-1.5 text-sm text-gray-700 hover:bg-gray-100 dark:border-gray-600 dark:text-gray-200 dark:hover:bg-gray-700" onclick={() => { close(); goto(resolve("/devices")); }}>Open Devices</button>
                            <button type="button" data-autofocus class="rounded-md bg-blue-600 px-3 py-1.5 text-sm font-medium text-white hover:bg-blue-700" onclick={close}>Done</button>
                        </div>
                    {:else}
                        {#if unstableId(tag.id)}
                            <p class="mb-4 rounded-md bg-amber-50 p-3 text-xs text-amber-900 dark:bg-amber-900/30 dark:text-amber-100" data-testid="unstable-id">
                                This ID may not last: the device might be rotating its address, or share this ID with others like it. Phones and watches need pairing instead:
                                <a class="underline" href={resolve("/devices")} onclick={close}>Enroll on the Devices page</a>.
                            </p>
                        {/if}
                        <div class="space-y-3">
                            <label class="block text-sm text-gray-700 dark:text-gray-300">
                                Name
                                <input data-autofocus bind:value={tagName} required class="mt-1" placeholder="Kitchen keys" />
                            </label>
                            <label class="block text-sm text-gray-700 dark:text-gray-300">
                                Type
                                <select bind:value={tagType} class="mt-1">
                                    <option value="">None</option>
                                    {#each deviceTypes as t}<option value={t}>{t}</option>{/each}
                                </select>
                            </label>
                            <label class="block text-sm text-gray-700 dark:text-gray-300">
                                ID to report
                                <input bind:value={tagAlias} class="mt-1 font-mono" placeholder={suggestAlias(tagName, tagType) || "keys:kitchen"} />
                            </label>
                        </div>
                        {#if saveError}<p class="mt-3 text-sm text-red-600 dark:text-red-400">{saveError}</p>{/if}
                        <div class="mt-5 flex justify-end gap-2">
                            <button type="button" class="rounded-md border border-gray-300 px-3 py-1.5 text-sm text-gray-700 hover:bg-gray-100 dark:border-gray-600 dark:text-gray-200 dark:hover:bg-gray-700" onclick={() => show({ view: "search" })}>Back</button>
                            <button type="submit" class="rounded-md bg-blue-600 px-3 py-1.5 text-sm font-medium text-white hover:bg-blue-700 disabled:opacity-50" disabled={!tagName.trim() || savingTag}>{savingTag ? "Saving…" : "Save"}</button>
                        </div>
                    {/if}
                </form>
            {/if}
        </div>
    </div>
{/if}
