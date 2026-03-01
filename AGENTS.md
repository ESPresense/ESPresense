# Repo Guidelines for Agents

* Run `npm install` inside the `ui` folder before using scripts.
* Use `npm run check` in `ui` to verify builds pass.
* Use `crow pipeline log show 1 <id> <sub_id>` to verify hardware in the loop builds pass.
* After modifying files under `ui`, execute `npm run build` in that folder to regenerate C++ headers under `src`.
