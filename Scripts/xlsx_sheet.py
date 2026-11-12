#!/usr/bin/env python3
"""Minimal XLSX sheet reader (no openpyxl): sharedStrings + inlineStr."""
from __future__ import annotations

import re
import zipfile
from pathlib import Path
from typing import List, Optional, Tuple
from xml.etree import ElementTree as ET

NS = {"m": "http://schemas.openxmlformats.org/spreadsheetml/2006/main"}
REL_NS = "{http://schemas.openxmlformats.org/officeDocument/2006/relationships}"


def _col_of(ref: str) -> int:
    m = re.match(r"([A-Z]+)", ref)
    n = 0
    for ch in m.group(1):
        n = n * 26 + (ord(ch) - 64)
    return n - 1


def read_sheet(path: Path, sheet_name: Optional[str] = None) -> Tuple[List[str], List[List[str]]]:
    with zipfile.ZipFile(path) as z:
        ss: List[str] = []
        if "xl/sharedStrings.xml" in z.namelist():
            root = ET.fromstring(z.read("xl/sharedStrings.xml"))
            for si in root.findall("m:si", NS):
                texts = [t.text or "" for t in si.findall(".//m:t", NS)]
                ss.append("".join(texts))

        wb = ET.fromstring(z.read("xl/workbook.xml"))
        sheets = []
        for sh in wb.findall("m:sheets/m:sheet", NS):
            sheets.append((sh.attrib.get("name"), sh.attrib.get(f"{REL_NS}id")))

        rels = ET.fromstring(z.read("xl/_rels/workbook.xml.rels"))
        rid_to_target = {r.attrib["Id"]: r.attrib["Target"] for r in rels}

        chosen = None
        if sheet_name:
            for name, rid in sheets:
                if name == sheet_name:
                    chosen = rid_to_target[rid]
                    break
        if chosen is None:
            name, rid = sheets[0]
            chosen = rid_to_target[rid]

        target = chosen.lstrip("/")
        if not target.startswith("xl/"):
            target = "xl/" + target if not target.startswith("worksheets") else "xl/" + target
        # Normalize common forms
        candidates = [target, "xl/" + Path(chosen).name, "xl/worksheets/" + Path(chosen).name]
        sheet_bytes = None
        for cand in candidates:
            if cand in z.namelist():
                sheet_bytes = z.read(cand)
                break
        if sheet_bytes is None:
            raise FileNotFoundError(f"sheet target not found: {chosen}")

        root = ET.fromstring(sheet_bytes)

        def cell_val(c) -> str:
            t = c.attrib.get("t")
            v = c.find("m:v", NS)
            is_elem = c.find("m:is", NS)
            if t == "s" and v is not None:
                return ss[int(v.text)]
            if t == "inlineStr" and is_elem is not None:
                return "".join(x.text or "" for x in is_elem.findall(".//m:t", NS))
            if v is not None and v.text is not None:
                return v.text
            return ""

        rows_out: List[List[str]] = []
        for row in root.findall("m:sheetData/m:row", NS):
            cells = {}
            for c in row.findall("m:c", NS):
                cells[_col_of(c.attrib["r"])] = cell_val(c)
            if not cells:
                continue
            mx = max(cells) + 1
            rows_out.append([cells.get(i, "") for i in range(mx)])

    if not rows_out:
        return [], []
    header = rows_out[0]
    data = rows_out[1:]
    return header, data
