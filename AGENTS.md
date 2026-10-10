# Repo Guidelines for Agents

* After modifying files under `ui`, execute `npm run build` in that folder to regenerate C++ headers under `main`.
* Pure ESP-IDF v5.4 project: `. ~/esp/esp-idf/export.sh && ./build.sh <env>` (see BUILDING.md). No Arduino, no PlatformIO.
* Flash with `tools/flash.sh <env> <port> [seconds]`: it flashes, then captures the serial log
  (saved to `build/<env>/serial.log`). Always show the user the post-flash log output, and
  confirm the board booted before calling a flash done. `tools/serial_log.py <port> [seconds]`
  reads the log alone. The script ends with the board's IP; always give it to the user. If the port is busy, the script prints which process holds it.
* Host unit tests: `./test/run.sh`.
* HIL (`.woodpecker/hil.yml`) runs 180s per device on a PR and 4h on main and the nightly
  cron. A memory or stability change needs the long run to prove anything, so ask for one:
  push the branch as `soak/<name>`, or put `[soak]` in the PR title (on pull_request events
  CI_COMMIT_MESSAGE is the PR title, not the head commit) and push any commit.

## HIL pipeline (CrowCI)

HIL runs on a CrowCI server; use the `crow` CLI, it's installed and `CROW_SERVER`/`CROW_TOKEN`
are already set in the shell. Repo is `ESPresense/ESPresense`; the pipeline number is the last
path segment of the `CrowCI pr - hil` link in `gh pr checks`. PRs from forks sit in pending
approval until `crow pipeline approve` is run.

`crow` is the successor to Woodpecker and speaks the same protocol, so every Woodpecker
reference still applies to Crow: the config lives in `.woodpecker/*.yml`, and the `WOODPECKER_*`
env vars (`CI_COMMIT_MESSAGE`, `CI_PIPELINE_EVENT`, ...) are what the steps see. `crow` also
accepts `WOODPECKER_SERVER`/`WOODPECKER_TOKEN` as aliases for its own.

`crow pipeline log show` takes a step name and ends in `PASS:`/`FAIL:`. Steps are `clone`,
`ble-flood`, `test-<target>` (one per device), `ble-flood-guard`; `crow pipeline ps` needs
`-o json` to show them (its table output is blank). A new push to the same PR kills the running
pipeline, so a soak result is on the latest number.

