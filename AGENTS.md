# Repo Guidelines for Agents

* After modifying files under `ui`, execute `npm run build` in that folder to regenerate C++ headers under `main`.
* Pure ESP-IDF v5.4 project: `. ~/esp/esp-idf/export.sh && ./build.sh <env>` (see BUILDING.md). No Arduino, no PlatformIO.
* Host unit tests: `./test/run.sh`.
* HIL (`.woodpecker/hil.yml`) runs 180s per device on a PR and 4h on main and the nightly
  cron. A memory or stability change needs the long run to prove anything, so ask for one:
  push the branch as `soak/<name>`, or put `[soak]` anywhere in the head commit message
  (an empty commit works, and unlike renaming a branch it will not close the PR).
