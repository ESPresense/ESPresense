# Repo Guidelines for Agents

* After modifying files under `ui`, execute `npm run build` in that folder to regenerate C++ headers under `main`.
* Pure ESP-IDF v5.4 project: `. ~/esp/esp-idf/export.sh && ./build.sh <env>` (see BUILDING.md). No Arduino, no PlatformIO.
* Host unit tests: `./test/run.sh`.
* HIL (`.woodpecker/hil.yml`) runs 180s per device on a PR and 4h on main and the nightly
  cron. A memory or stability change needs the long run to prove anything, so ask for one:
  push the branch as `soak/<name>`, or put `[soak]` in the PR title (on pull_request events
  Woodpecker's CI_COMMIT_MESSAGE is the PR title, not the head commit) and push any commit.

## CrowCI CLI

`crow` (Homebrew) talks to the HIL server; `CROW_SERVER` / `CROW_TOKEN` are set in the shell.
Repo is `ESPresense/ESPresense`; the pipeline number is the last path segment of the
`CrowCI pr - hil` link in `gh pr checks`. PRs from forks sit in "pending approval" until approved.

```sh
crow pipeline ls ESPresense/ESPresense --event pull_request --limit 5   # recent runs (--branch, --status)
crow pipeline approve ESPresense/ESPresense <n>                          # fork PR gate
crow pipeline ps ESPresense/ESPresense <n> -o json | jq -r '.[] | "\(.name) \(.state)"'   # table output is blank
crow pipeline log show ESPresense/ESPresense <n> test-esp32c3            # by step name; ends in PASS:/FAIL:
crow pipeline start ESPresense/ESPresense <n>                            # rerun
```

Steps are `clone`, `ble-flood`, `test-<target>` (one per device), `ble-flood-guard`. A new push
to the same PR kills the running pipeline (status `killed`), so a soak result is on the latest number.
