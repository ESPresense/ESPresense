# manifest-check

Headless probe of the public ESP Web Tools manifest endpoints used by the
ESPresense web installer. Reproduces issue
[#2225](https://github.com/ESPresense/ESPresense/issues/2225) ("Failed to
download manifest"), where `https://espresense.com/releases/{version}.json?flavor={flavor}`
intermittently returns 500 and the web installer surfaces it as a generic
"Failed to download manifest" toast with no diagnostic.

## Run locally

```sh
python3 tools/manifest_check/check_manifest.py
```

Skip the per-binary HEAD validation (faster, useful when iterating on the
script itself):

```sh
python3 tools/manifest_check/check_manifest.py --no-check-parts
```

Probe a different host (for staging / mirror tests):

```sh
python3 tools/manifest_check/check_manifest.py --base https://staging.espresense.com
```

## What the probe checks

For each `version,flavor` listed in [`targets.txt`](./targets.txt):

1. `GET /releases/{version}.json?flavor={flavor}` returns HTTP 200 with
   `application/json`.
2. The body parses as JSON.
3. Required top-level keys are present (`name`, `version`, `builds`).
4. Each `builds[]` entry has a non-empty `parts[]` array with `path` and
   `offset`.
5. Every `parts[].path` HEAD-resolves to 200 / 3xx (catches stale binaries
   removed from the CDN).

Single transient 5xx responses are retried up to 3 times with exponential
backoff before failing the run; this keeps CI from flapping on cloudfront
hiccups while still catching persistent 500s like the one reported on
[#2225](https://github.com/ESPresense/ESPresense/issues/2225).

## Updating the target list

[`targets.txt`](./targets.txt) is the source of truth for which (version,
flavor) pairs the installer is expected to serve. Edit it when:

- A new release ships and is added to the installer dropdown.
- A new flavor (board / variant) enters the installer dropdown.
- A release is removed from the public installer (e.g., yanked due to a
  bricking bug).

Lines starting with `#` are comments. Format is `version,flavor` per line.

## CI

[`.github/workflows/manifest-check.yml`](../../.github/workflows/manifest-check.yml)
runs the probe on:

- pull requests that touch this directory or the workflow,
- pushes to `main` that touch this directory or the workflow,
- an hourly cron (`17 * * * *`),
- manual `workflow_dispatch`.
