#!/usr/bin/env python3
"""Sync Libraries/Rdk-HardwareLib/Catalog → Bin/HardwareCatalog (or --dest)."""
from __future__ import annotations

import argparse
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SRC = ROOT / "Catalog"
DEFAULT_DEST = Path(__file__).resolve().parents[3] / "Bin" / "HardwareCatalog"


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument(
        "--dest",
        type=Path,
        default=None,
        help="Destination HardwareCatalog root (default: repo Bin/HardwareCatalog)",
    )
    args = ap.parse_args()
    dest = args.dest or DEFAULT_DEST
    if not SRC.is_dir():
        print(f"missing source {SRC}", file=sys.stderr)
        return 1
    dest.mkdir(parents=True, exist_ok=True)
    # Copy tree — replace JSON/assets under dest, preserve unrelated
    for path in SRC.rglob("*"):
        if path.is_dir():
            continue
        rel = path.relative_to(SRC)
        out = dest / rel
        out.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(path, out)
    print(f"synced {SRC} -> {dest}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
