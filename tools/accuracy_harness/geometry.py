"""Room / station / track geometry primitives.

Scenarios are loaded from JSON files under `tools/accuracy_harness/scenarios/`.
A scenario describes:

  - bounds: floor bounding box (xy meters)
  - rooms: named axis-aligned rectangles for room-misidentification scoring
  - stations: (id, x, y, z, cal_rssi, absorption) per ESPresense node
  - track: ordered list of (x, y) waypoints traversed at constant speed

JSON is preferred over YAML to avoid an extra runtime dependency.
"""

from __future__ import annotations

import json
from dataclasses import dataclass, field
from pathlib import Path


@dataclass(frozen=True)
class Station:
    id: str
    x: float
    y: float
    z: float
    cal_rssi: float
    absorption: float


@dataclass(frozen=True)
class Room:
    name: str
    x_min: float
    y_min: float
    x_max: float
    y_max: float

    def contains(self, x: float, y: float) -> bool:
        return self.x_min <= x <= self.x_max and self.y_min <= y <= self.y_max


@dataclass(frozen=True)
class Bounds:
    x_min: float
    y_min: float
    x_max: float
    y_max: float


@dataclass(frozen=True)
class Track:
    name: str
    points: tuple[tuple[float, float], ...]
    samples_per_segment: int = 10

    def expand(self) -> list[tuple[float, float, str]]:
        """Linearly interpolate waypoints, returning (x, y, label) per sample.

        `label` is the segment index as a string, useful for plotting/debug.
        """
        out: list[tuple[float, float, str]] = []
        for i in range(len(self.points) - 1):
            x0, y0 = self.points[i]
            x1, y1 = self.points[i + 1]
            for s in range(self.samples_per_segment):
                t = s / self.samples_per_segment
                out.append((x0 + (x1 - x0) * t, y0 + (y1 - y0) * t, str(i)))
        out.append((*self.points[-1], str(len(self.points) - 1)))
        return out


@dataclass(frozen=True)
class Scenario:
    name: str
    bounds: Bounds
    rooms: tuple[Room, ...]
    stations: tuple[Station, ...]
    tracks: tuple[Track, ...]
    metadata: dict = field(default_factory=dict)

    def room_for(self, x: float, y: float) -> str | None:
        for r in self.rooms:
            if r.contains(x, y):
                return r.name
        return None


def load_scenario(path: Path) -> Scenario:
    raw = json.loads(Path(path).read_text())
    bounds = Bounds(**raw["bounds"])
    rooms = tuple(Room(**r) for r in raw.get("rooms", []))
    stations = tuple(Station(**s) for s in raw["stations"])
    tracks = tuple(
        Track(
            name=t["name"],
            points=tuple((p[0], p[1]) for p in t["points"]),
            samples_per_segment=t.get("samples_per_segment", 10),
        )
        for t in raw["tracks"]
    )
    return Scenario(
        name=raw["name"],
        bounds=bounds,
        rooms=rooms,
        stations=stations,
        tracks=tracks,
        metadata=raw.get("metadata", {}),
    )
