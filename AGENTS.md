# Repo Guidelines for Agents

* Run `npm install` inside the `ui` folder before using scripts.
* Use `npm run check` in `ui` to verify builds pass.
* Use `crow pipeline log show 1 <id> <sub_id>` to verify hardware in the loop builds pass.
* After modifying files under `ui`, execute `npm run build` in that folder to regenerate C++ headers under `src`.
* `pio-on && pio` to run platform io
* HIL (`.woodpecker/hil.yml`) runs 180s per device on a PR and 4h on main and the nightly
  cron. A memory or stability change needs the long run to prove anything, so ask for one:
  push the branch as `soak/<name>`, or put `[soak]` anywhere in the head commit message
  (an empty commit works, and unlike renaming a branch it will not close the PR).
