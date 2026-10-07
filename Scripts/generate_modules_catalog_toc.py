#!/usr/bin/env python3
"""Regenerate Modules-Catalog.md TOC grouped by category with runtime badges."""
from __future__ import annotations

import json
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MOD_DIR = ROOT / "Catalog" / "modules"
OUT = ROOT / "Docs" / "Modules-Catalog.md"


def main() -> int:
    by_cat: dict[str, list[tuple[str, str, str]]] = defaultdict(list)
    for path in sorted(MOD_DIR.glob("*.json")):
        data = json.loads(path.read_text(encoding="utf-8"))
        mid = data.get("id") or path.stem
        title = data.get("title") or mid
        cat = data.get("category") or "Прочее"
        runtime = data.get("runtime") or "planned"
        by_cat[cat].append((mid, title, runtime))

    lines: list[str] = [
        "# Modules Catalog",
        "",
        "## RU",
        "",
        "Каталог модулей Arduino/ESP32 импортирован из",
        "[`Docs/ArduinoShields/arduino-esp32-modules-catalog.xlsx`](ArduinoShields/arduino-esp32-modules-catalog.xlsx)",
        "скриптом [`Scripts/import_modules_catalog_xlsx.py`](../Scripts/import_modules_catalog_xlsx.py).",
        "",
        "JSON: `Catalog/modules/<id>.json`. Индекс: `Catalog/catalog.json`.",
        "Зеркало runtime: `Bin/HardwareCatalog` (`Scripts/sync_hardware_catalog.py`).",
        "",
        "Поле `runtime`: `firmata` | `hub` | `motor_hub` | `planned`.",
        "",
        "Перегенерация TOC:",
        "",
        "```bash",
        "cd Libraries/Rdk-HardwareLib/Scripts",
        "python3 import_modules_catalog_xlsx.py",
        "python3 generate_modules_catalog_toc.py",
        "python3 sync_hardware_catalog.py --dest /path/to/Bin/HardwareCatalog",
        "```",
        "",
        "### TOC по Category",
        "",
    ]
    for cat in sorted(by_cat.keys(), key=lambda c: (-len(by_cat[c]), c)):
        lines.append(f"#### {cat} ({len(by_cat[cat])})")
        lines.append("")
        lines.append("| id | title | runtime |")
        lines.append("|----|-------|---------|")
        for mid, title, runtime in sorted(by_cat[cat], key=lambda t: t[0]):
            lines.append(f"| `{mid}` | {title} | `{runtime}` |")
        lines.append("")

    lines.extend(
        [
            "## EN",
            "",
            "Module JSON catalog generated from the xlsx sheet «Каталог».",
            "See RU section for paths, runtime badges, and the category TOC.",
            "",
        ]
    )
    OUT.write_text("\n".join(lines), encoding="utf-8")
    print(f"wrote {OUT} ({sum(len(v) for v in by_cat.values())} modules)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
