#!/usr/bin/env python3
"""Generate in-house Arduino pinout SVG + matching pins.json (physical header layout)."""
from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parent


def pin(pid, x, y, w, h, firmata, kind="digital"):
    return {
        "id": pid,
        "label": pid,
        "rect": [round(x, 4), round(y, 4), round(w, 4), round(h, 4)],
        "firmataPin": firmata,
        "kind": kind,
    }


def row_pins(ids, firmata, kinds, x0, y, w, h, gap, extra_gaps=None):
    extra_gaps = extra_gaps or {}
    x = x0
    out = []
    for i, pid in enumerate(ids):
        if i in extra_gaps:
            x += extra_gaps[i]
        out.append(pin(pid, x, y, w, h, firmata[i], kinds[i]))
        x += w + gap
    return out


def svg_header(vb_w, vb_h):
    return (
        f'<?xml version="1.0" encoding="UTF-8"?>\n'
        f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {vb_w} {vb_h}">\n'
    )


def hole(x, y, w, h, vb_w, vb_h):
    px, py, pw, ph = x * vb_w, y * vb_h, w * vb_w, h * vb_h
    return (
        f'  <rect x="{px:.2f}" y="{py:.2f}" width="{pw:.2f}" height="{ph:.2f}" '
        f'rx="{pw * 0.35:.2f}" fill="#1b5e20" stroke="#cfd8dc" stroke-width="0.6"/>\n'
    )


def silk(x, y, text, vb_w, vb_h, size=7, fill="#e3f2fd", anchor="middle"):
    return (
        f'  <text x="{x * vb_w:.2f}" y="{y * vb_h:.2f}" text-anchor="{anchor}" '
        f'fill="{fill}" font-size="{size}" font-family="sans-serif">{text}</text>\n'
    )


def write_uno():
    vb_w, vb_h = 400.0, 200.0
    w, h, gap = 0.028, 0.07, 0.008
    top_y, bot_y = 0.145, 0.78
    d_ids = [f"D{n}" for n in range(13, -1, -1)]  # D13 .. D0, USB-left
    d_firm = list(range(13, -1, -1))
    d_kind = ["digital"] * 14
    analog = [pin(f"A{i}", 0.42 + i * (w + gap + 0.004), bot_y, w, h, 14 + i, "analog") for i in range(6)]
    digital = row_pins(d_ids, d_firm, d_kind, 0.22, top_y, w, h, gap, extra_gaps={6: 0.03})
    pins = digital + analog
    doc = {"viewBox": [0, 0, int(vb_w), int(vb_h)], "usbRect": [0.02, 0.35, 0.07, 0.22], "pins": pins}
    (ROOT / "uno_pins.json").write_text(json.dumps(doc, indent=2) + "\n", encoding="utf-8")

    s = svg_header(vb_w, vb_h)
    s += '  <rect x="40" y="22" width="340" height="156" rx="10" fill="#00979d" stroke="#263238" stroke-width="2"/>\n'
    s += '  <rect x="8" y="78" width="30" height="44" rx="4" fill="#37474f"/>\n'
    s += silk(0.057, 0.53, "USB", vb_w, vb_h, 8, "#eceff1")
    s += silk(0.52, 0.40, "Arduino Uno R3", vb_w, vb_h, 16, "#ffffff")
    s += silk(0.22, 0.12, "D13", vb_w, vb_h, 7, "#fff59d", "start")
    s += silk(0.78, 0.12, "D0", vb_w, vb_h, 7, "#fff59d", "end")
    s += silk(0.42, 0.93, "A0", vb_w, vb_h, 7, "#81d4fa", "start")
    s += silk(0.62, 0.93, "A5", vb_w, vb_h, 7, "#81d4fa", "end")
    for p in pins:
        x, y, pw, ph = p["rect"]
        s += hole(x, y, pw, ph, vb_w, vb_h)
    s += "</svg>\n"
    (ROOT / "arduino_uno_pinout.svg").write_text(s, encoding="utf-8")


def write_mega():
    vb_w, vb_h = 640.0, 260.0
    w, h, gap = 0.018, 0.055, 0.004
    top_y, bot_y = 0.12, 0.80
    # Top: D13..D0 then D14..D21 (comms), USB on the left
    top_ids = [f"D{n}" for n in range(13, -1, -1)] + [f"D{n}" for n in range(14, 22)]
    top_firm = list(range(13, -1, -1)) + list(range(14, 22))
    top_kind = ["digital"] * 22
    digital_top = row_pins(top_ids, top_firm, top_kind, 0.14, top_y, w, h, gap, extra_gaps={6: 0.018, 14: 0.02})
    analog = [
        pin(f"A{i}", 0.16 + i * (w + gap + 0.002), bot_y, w, h, 54 + i, "analog") for i in range(16)
    ]
    # Right dual-row D22-D53 (even inner, odd outer), top to bottom
    right = []
    for i in range(16):
        even, odd = 22 + i * 2, 23 + i * 2
        y = 0.16 + i * 0.042
        right.append(pin(f"D{even}", 0.86, y, 0.028, 0.036, even, "digital"))
        right.append(pin(f"D{odd}", 0.90, y, 0.028, 0.036, odd, "digital"))
    pins = digital_top + analog + right
    doc = {"viewBox": [0, 0, int(vb_w), int(vb_h)], "usbRect": [0.015, 0.36, 0.055, 0.22], "pins": pins}
    (ROOT / "mega2560_pins.json").write_text(json.dumps(doc, indent=2) + "\n", encoding="utf-8")

    s = svg_header(vb_w, vb_h)
    s += '  <rect x="48" y="28" width="560" height="204" rx="12" fill="#00979d" stroke="#263238" stroke-width="2"/>\n'
    s += '  <rect x="10" y="100" width="36" height="56" rx="4" fill="#37474f"/>\n'
    s += silk(0.043, 0.50, "USB", vb_w, vb_h, 9, "#eceff1")
    s += silk(0.48, 0.48, "Arduino Mega 2560", vb_w, vb_h, 18, "#ffffff")
    s += silk(0.14, 0.095, "D13", vb_w, vb_h, 8, "#fff59d", "start")
    s += silk(0.62, 0.095, "D0", vb_w, vb_h, 8, "#fff59d", "end")
    s += silk(0.70, 0.095, "D14-21", vb_w, vb_h, 8, "#ce93d8", "start")
    s += silk(0.16, 0.94, "A0", vb_w, vb_h, 8, "#81d4fa", "start")
    s += silk(0.52, 0.94, "A15", vb_w, vb_h, 8, "#81d4fa", "end")
    s += silk(0.88, 0.12, "D22", vb_w, vb_h, 8, "#a5d6a7", "start")
    s += silk(0.88, 0.78, "D53", vb_w, vb_h, 8, "#a5d6a7", "start")
    for p in pins:
        x, y, pw, ph = p["rect"]
        s += hole(x, y, pw, ph, vb_w, vb_h)
    s += "</svg>\n"
    (ROOT / "arduino_mega2560_pinout.svg").write_text(s, encoding="utf-8")


if __name__ == "__main__":
    write_uno()
    write_mega()
    print("wrote uno + mega pinout svg/json")
