#!/usr/bin/env python3
"""Fetch the unchanged, pinned ESPectre SDK for the isolated csiref build."""
import hashlib
import io
import json
from pathlib import Path
import shutil
import tarfile
import urllib.request

REVISION = "3edc7bdb4262e01b868c3d589fea59cfd0e8f555"
ROOT = Path(__file__).resolve().parents[1]


def main():
    dest = ROOT / ".reference" / "espectre"
    marker = dest / "reference-source.json"
    if marker.exists() and json.loads(marker.read_text())["revision"] == REVISION:
        print(f"ESPectre SDK already prepared: {REVISION}")
        return
    url = f"https://codeload.github.com/francescopace/espectre/tar.gz/{REVISION}"
    with urllib.request.urlopen(url, timeout=90) as response:
        data = response.read()
    staging = dest.with_name("espectre-staging")
    staging.mkdir(parents=True, exist_ok=False)
    try:
        prefix = f"espectre-{REVISION}/"
        with tarfile.open(fileobj=io.BytesIO(data), mode="r:gz") as archive:
            for member in archive:
                relative = member.name.removeprefix(prefix)
                if relative.startswith("src/cpp/"):
                    relative = relative.removeprefix("src/cpp/")
                elif relative not in ("LICENSE", "LICENSING.md", "THIRD_PARTY_NOTICES.md"):
                    continue
                if not member.isfile():
                    continue
                target = staging / relative
                if not target.resolve().is_relative_to(staging.resolve()):
                    raise ValueError("Unsafe archive path")
                target.parent.mkdir(parents=True, exist_ok=True)
                target.write_bytes(archive.extractfile(member).read())
        source = {"repository": "https://github.com/francescopace/espectre", "revision": REVISION,
                  "archive_sha256": hashlib.sha256(data).hexdigest()}
        (staging / "reference-source.json").write_text(json.dumps(source, indent=2) + "\n")
        if dest.exists():
            raise RuntimeError("Existing SDK differs; preserve it and use a fresh checkout")
        staging.rename(dest)
        print(json.dumps(source))
    except BaseException:
        shutil.rmtree(staging)
        raise


if __name__ == "__main__":
    main()
