#!/usr/bin/env python3
"""Import Arduino/ESP32 modules catalog xlsx → Catalog/modules (+ shields for Шилды)."""
from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path

from xlsx_sheet import read_sheet

ROOT = Path(__file__).resolve().parents[1]
CATALOG = ROOT / "Catalog"
DEFAULT_XLSX = ROOT / "Docs/ArduinoShields/arduino-esp32-modules-catalog.xlsx"
MOTOR_XLSX = ROOT / "Docs/ArduinoShields/arduino-motor-shields-comparison.xlsx"

# Preserve known ids when name matches
KNOWN_ID_BY_TITLE = {
    "dht11": "dht11",
    "hc-sr04": "hc_sr04",
    "hc_sr04": "hc_sr04",
    "ultrasonic hc-sr04": "hc_sr04",
}


def slugify(name: str) -> str:
    s = name.lower().strip()
    s = s.replace("ё", "e")
    s = re.sub(r"[^\w\s\-/+]", "", s, flags=re.UNICODE)
    s = s.replace("/", " ").replace("+", "plus").replace("-", " ")
    s = re.sub(r"\s+", "_", s.strip())
    s = re.sub(r"_+", "_", s).strip("_")
    if not s:
        s = "module"
    if s[0].isdigit():
        s = "m_" + s
    return s[:64]


def infer_runtime(interface: str, category: str) -> str:
    iface = (interface or "").lower()
    cat = (category or "").lower()
    if "motor" in cat or "мотор" in cat:
        return "motor_hub"
    if "i2c" in iface or "spi" in iface:
        return "planned"
    if "1-wire" in iface or "gpio" in iface or "analog" in iface or "pwm" in iface:
        return "firmata"
    if "uart" in iface or "serial" in iface:
        return "hub"
    return "planned"


def infer_port_kind(interface: str) -> str:
    iface = (interface or "").lower()
    if "analog" in iface:
        return "analog"
    if "i2c" in iface:
        return "i2c"
    if "spi" in iface:
        return "spi"
    if "pwm" in iface:
        return "pwm"
    return "digital"


def infer_signal_type(interface: str, name: str) -> str:
    iface = (interface or "").lower()
    n = (name or "").lower()
    if "dht" in n:
        return "digital_1wire_timing"
    if "sr04" in n or "ultrasonic" in n:
        return "digital_echo_timing"
    if "analog" in iface:
        return "analog"
    if "pwm" in iface:
        return "pwm"
    if "i2c" in iface:
        return "i2c"
    if "spi" in iface:
        return "spi"
    return "digital"


def infer_roles(category: str) -> list:
    c = (category or "").lower()
    if "датчик" in c or "sensor" in c:
        return ["sensor"]
    if "мотор" in c or "motor" in c or "привод" in c:
        return ["actuator"]
    if "дисплей" in c or "display" in c:
        return ["display"]
    if "беспровод" in c or "wireless" in c or "радио" in c:
        return ["radio"]
    if "шилд" in c or "shield" in c:
        return ["shield"]
    return ["module"]


def merge_module(existing: dict, generated: dict) -> dict:
    """Keep runtime-critical fields from existing modules; enrich metadata from xlsx."""
    out = dict(generated)
    if not existing:
        return out
    for key in (
        "signalType",
        "wires",
        "roles",
        "portKind",
        "channelIds",
        "requiresMcuTiming",
        "requiredCapabilities",
        "preferredFirmware",
        "asset",
    ):
        if key in existing and existing[key] not in (None, "", [], {}):
            out[key] = existing[key]
    # Keep id/title from existing if present
    out["id"] = existing.get("id", out["id"])
    if existing.get("title"):
        out["title"] = existing["title"]
    return out


def load_json(path: Path) -> dict:
    if path.is_file():
        return json.loads(path.read_text(encoding="utf-8"))
    return {}


def write_json(path: Path, obj: dict) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(obj, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


def import_modules(xlsx: Path) -> list:
    header, rows = read_sheet(xlsx, sheet_name="Каталог")
    col = {h: i for i, h in enumerate(header)}

    def get(row, key, default=""):
        i = col.get(key)
        if i is None or i >= len(row):
            return default
        return (row[i] or "").strip()

    written = []
    seen_ids = set()
    for row in rows:
        name = get(row, "Name")
        if not name:
            continue
        category = get(row, "Category")
        chip = get(row, "Chip / IC")
        platform = get(row, "Platform")
        interface = get(row, "Interface")
        supply = get(row, "Typical supply voltage")
        desc = get(row, "Short description (RU)")
        aliases_raw = get(row, "Compatible clones / aliases")
        notes = get(row, "Notes")
        urls = {
            "buy": get(row, "Buy URL"),
            "datasheet": get(row, "Datasheet URL"),
            "docs": get(row, "Documentation URL"),
            "image": get(row, "Product image URL"),
            "wiring": get(row, "Wiring / pinout URL"),
        }
        urls = {k: v for k, v in urls.items() if v}

        base_slug = slugify(name.split("/")[0].strip())
        # Known mappings
        key = name.lower().replace(" ", "")
        mid = None
        for kn, kid in KNOWN_ID_BY_TITLE.items():
            if kn.replace("_", "").replace("-", "") in key.replace("_", "").replace("-", ""):
                if kn in ("dht11",) and "dht11" in key and "dht22" not in key:
                    mid = kid
                    break
                if kn in ("hc-sr04", "hc_sr04") and ("sr04" in key or "hc_sr04" in key):
                    mid = kid
                    break
        if mid is None:
            # match existing files by fuzzy title
            mid = base_slug

        # Ensure uniqueness
        candidate = mid
        n = 2
        while candidate in seen_ids:
            candidate = f"{mid}_{n}"
            n += 1
        mid = candidate
        seen_ids.add(mid)

        path = CATALOG / "modules" / f"{mid}.json"
        existing = load_json(path)

        generated = {
            "schemaVersion": 1,
            "id": mid,
            "title": existing.get("title") or name.split("/")[0].strip(),
            "category": category,
            "chip": chip,
            "platform": platform,
            "interface": interface,
            "supply": supply,
            "descriptionRu": desc,
            "aliases": [a.strip() for a in aliases_raw.split(",") if a.strip()],
            "urls": urls,
            "notes": notes,
            "runtime": infer_runtime(interface, category),
            "signalType": infer_signal_type(interface, name),
            "portKind": infer_port_kind(interface),
            "roles": infer_roles(category),
            "wires": existing.get("wires") or ["G", "V", "S"],
            "preferredFirmware": existing.get("preferredFirmware")
            or (["standard_firmata"] if infer_runtime(interface, category) == "firmata" else []),
            "requiresMcuTiming": bool(existing.get("requiresMcuTiming", False)),
            "requiredCapabilities": existing.get("requiredCapabilities") or [],
            "asset": existing.get("asset") or "",
        }
        if "шилд" in category.lower() or "shield" in category.lower():
            generated["formFactor"] = "shield"
            generated["runtime"] = "firmata"

        out = merge_module(existing, generated)
        # Keep id from path/existing if known file
        if existing.get("id"):
            out["id"] = existing["id"]
            # rewrite under existing id
            if existing["id"] != mid:
                seen_ids.discard(mid)
                seen_ids.add(existing["id"])
                path = CATALOG / "modules" / f"{existing['id']}.json"
                mid = existing["id"]

        write_json(path, out)
        written.append(mid)

        # Shields category → also Catalog/shields stub if missing
        if out.get("formFactor") == "shield" or "шилд" in category.lower():
            sp = CATALOG / "shields" / f"{mid}.json"
            if not sp.is_file():
                write_json(
                    sp,
                    {
                        "schemaVersion": 1,
                        "id": mid,
                        "title": out["title"],
                        "compatibleBoards": ["uno", "mega2560", "esp32_devkit"],
                        "formFactor": "uno_r3_shield",
                        "defaultFirmware": "standard_firmata",
                        "layoutAsset": "",
                        "compatibleModules": [],
                        "channels": {},
                        "occupiesPins": [],
                        "ports": {},
                        "notes": notes,
                    },
                )
    return written


def import_motor_drivers(xlsx: Path) -> list:
    header, rows = read_sheet(xlsx, sheet_name="Сравнение")
    col = {h: i for i, h in enumerate(header)}

    def get(row, key, default=""):
        i = col.get(key)
        if i is None or i >= len(row):
            return default
        return (row[i] or "").strip()

    # Map known titles to shield ids
    TITLE_TO_ID = {
        "arduino motor shield rev3": "motor_shield_r3",
        "amperka motor shield": "motor_shield_amperka",
        "amperka motor shield plus": "motor_shield_amperka_plus",
        "seeed motor shield v1": "motor_shield_seeed_v1",
        "seeed grove i2c motor driver": "grove_i2c_motor_driver",
        "l298n dual h-bridge module": "wire_l298n",
        "tb6612fng dual motor driver": "tb6612fng",
        "drv8833 dual motor driver": "drv8833",
        "a4988 stepper driver": "a4988",
        "tmc2208 uart stepper": "tmc2208",
    }

    written = []
    for row in rows:
        title = get(row, "Название")
        if not title:
            continue
        form = get(row, "Формфактор")
        chip = get(row, "Драйвер (чип)")
        motors = get(row, "Моторы")
        power = get(row, "Ток / напряжение")
        notes = get(row, "Примечания")
        key = title.lower().strip()
        mid = TITLE_TO_ID.get(key) or slugify(title)

        control = "dir_pwm"
        if "stepper" in motors.lower() or "step" in chip.lower() or "a4988" in mid or "tmc" in mid:
            control = "step_dir"
        if "i2c" in title.lower() or "i2c" in notes.lower():
            control = "i2c"

        path = CATALOG / "shields" / f"{mid}.json"
        existing = load_json(path)
        obj = {
            "schemaVersion": 1,
            "id": mid,
            "title": title,
            "compatibleBoards": existing.get("compatibleBoards")
            or ["uno", "mega2560", "esp32_devkit", "esp32_uno_formfactor"],
            "formFactor": form or existing.get("formFactor") or "module",
            "driverChip": chip,
            "motors": motors,
            "power": power,
            "controlModel": existing.get("controlModel") or control,
            "defaultFirmware": existing.get("defaultFirmware") or "nmsdk_motor_hub_v1",
            "layoutAsset": existing.get("layoutAsset") or "",
            "compatibleModules": existing.get("compatibleModules") or ["dc_motor_channel"],
            "channels": existing.get("channels") or {},
            "occupiesPins": existing.get("occupiesPins") or [],
            "ports": existing.get("ports") or {},
            "notes": notes,
            "urls": {
                "buy": get(row, "Купить"),
                "datasheet": get(row, "Datasheet"),
                "docs": get(row, "Документация / wiki / library"),
            },
        }
        # Drop empty urls
        obj["urls"] = {k: v for k, v in obj["urls"].items() if v}
        if existing.get("channels"):
            obj["channels"] = existing["channels"]
        if existing.get("occupiesPins"):
            obj["occupiesPins"] = existing["occupiesPins"]
        write_json(path, obj)
        written.append(mid)

        # Also a module entry for non-shield form factors
        if form and form.lower() != "shield":
            mp = CATALOG / "modules" / f"{mid}.json"
            me = load_json(mp)
            write_json(
                mp,
                merge_module(
                    me,
                    {
                        "schemaVersion": 1,
                        "id": mid,
                        "title": title,
                        "category": "Моторы / драйверы",
                        "chip": chip,
                        "platform": "both",
                        "interface": control,
                        "supply": power,
                        "descriptionRu": notes,
                        "aliases": [],
                        "urls": obj.get("urls") or {},
                        "notes": notes,
                        "runtime": "motor_hub",
                        "signalType": "motor_driver",
                        "portKind": "digital",
                        "roles": ["actuator"],
                        "wires": ["G", "V", "DIR", "PWM"],
                        "preferredFirmware": ["nmsdk_motor_hub_v1"],
                        "requiresMcuTiming": False,
                        "requiredCapabilities": [],
                        "asset": "",
                        "controlModel": control,
                    },
                ),
            )
    return written


def rebuild_catalog_index() -> None:
    index_path = CATALOG / "catalog.json"
    index = load_json(index_path) or {"schemaVersion": 1, "name": "nmsdk-hardware-catalog"}

    def list_rel(subdir: str) -> list:
        d = CATALOG / subdir
        files = sorted(p.name for p in d.glob("*.json"))
        return [f"{subdir}/{n}" for n in files]

    index["boards"] = list_rel("boards")
    index["shields"] = list_rel("shields")
    index["modules"] = list_rel("modules")
    # keep firmwares order if present, else scan
    if not index.get("firmwares"):
        index["firmwares"] = list_rel("firmwares")
    else:
        # ensure all firmware files listed
        have = set(index["firmwares"])
        for rel in list_rel("firmwares"):
            if rel not in have:
                index["firmwares"].append(rel)
    write_json(index_path, index)


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--xlsx", type=Path, default=DEFAULT_XLSX)
    ap.add_argument("--motor-xlsx", type=Path, default=MOTOR_XLSX)
    ap.add_argument("--skip-motors", action="store_true")
    args = ap.parse_args()

    mods = import_modules(args.xlsx)
    print(f"modules written/updated: {len(mods)}")
    if not args.skip_motors and args.motor_xlsx.is_file():
        motors = import_motor_drivers(args.motor_xlsx)
        print(f"motor drivers written/updated: {len(motors)}")
    rebuild_catalog_index()
    index = load_json(CATALOG / "catalog.json")
    print(
        f"catalog.json boards={len(index.get('boards', []))} "
        f"shields={len(index.get('shields', []))} "
        f"modules={len(index.get('modules', []))}"
    )
    return 0


if __name__ == "__main__":
    sys.path.insert(0, str(Path(__file__).resolve().parent))
    raise SystemExit(main())
