"""CLI entry: `python -m accuracy_harness baseline`.

Run from the firmware repo root:

    python -m tools.accuracy_harness baseline

Or, with `tools/` on PYTHONPATH:

    PYTHONPATH=tools python -m accuracy_harness baseline
"""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

from . import __version__
from .geometry import load_scenario
from .noise import NoiseProfile
from .report import to_json, to_markdown
from .runner import run_scenario


HERE = Path(__file__).resolve().parent
SCENARIOS_DIR = HERE / "scenarios"
REPORTS_DIR = HERE / "reports"

BASELINE_SCENARIOS = ["baseline_4node_6x4m.json"]


def _cmd_baseline(args: argparse.Namespace) -> int:
    REPORTS_DIR.mkdir(parents=True, exist_ok=True)
    profile = NoiseProfile(
        gaussian_std_db=args.gaussian_std_db,
        multipath_probability=args.multipath_probability,
        multipath_attenuation_db=args.multipath_attenuation_db,
    )

    reports = []
    for fname in args.scenarios or BASELINE_SCENARIOS:
        path = SCENARIOS_DIR / fname
        scn = load_scenario(path)
        reports.append(run_scenario(scn, base_seed=args.seed, noise=profile))

    json_payload = to_json(reports)
    md_payload = to_markdown(reports)

    if args.output:
        out = Path(args.output)
        out.write_text(json_payload)
        print(f"Wrote {out}", file=sys.stderr)
    else:
        sys.stdout.write(json_payload)

    if args.markdown:
        md_path = Path(args.markdown)
        md_path.write_text(md_payload)
        print(f"Wrote {md_path}", file=sys.stderr)

    return 0


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(
        prog="accuracy_harness",
        description="ESPresense simulated trilateration accuracy harness",
    )
    parser.add_argument("--version", action="version", version=__version__)
    sub = parser.add_subparsers(dest="cmd", required=True)

    base = sub.add_parser("baseline", help="run the tier-1 baseline scenario set")
    base.add_argument("--seed", type=int, default=20260426)
    base.add_argument("--gaussian-std-db", type=float, default=2.5)
    base.add_argument("--multipath-probability", type=float, default=0.10)
    base.add_argument("--multipath-attenuation-db", type=float, default=6.0)
    base.add_argument("--scenarios", nargs="*", help="scenario filenames under scenarios/")
    base.add_argument("--output", help="write JSON report to this path")
    base.add_argument("--markdown", help="write markdown report to this path")
    base.set_defaults(func=_cmd_baseline)

    args = parser.parse_args(argv)
    return args.func(args)


if __name__ == "__main__":
    raise SystemExit(main())
