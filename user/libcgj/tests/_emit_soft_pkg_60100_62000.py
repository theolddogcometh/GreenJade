#!/usr/bin/env python3
"""Emit GreenJade soft packages M=60100..62000 (mirror 25800; bar3=0).

CREATE-ONLY: skips existing files. Safe to re-run.
"""
from __future__ import annotations

import os
from pathlib import Path

HERE = Path(__file__).resolve().parent
SRC_C = HERE / "cgj_soft_milestone_25800.c"
SRC_N = HERE / "NOTES_milestone_25800.txt"

# Decade soft-probe list in 25800 templates freezes at 14500..25700 siblings.
# For higher M we keep that frozen sibling header (as 60000 does) and only
# rewire M-specific decade markers / batch IDs.


def rewrite_from_25800(text: str, m: int) -> str:
    """Map 25800 soft package text onto milestone m with correct decade wave."""
    # Decade exclusive markers: (m-9) .. m
    # Fill: (m-24) .. (m-10)
    # Identity: (m-49) .. (m-25)
    d0 = m - 9  # first exclusive marker batch
    fill0, fill1 = m - 24, m - 10
    id0, id1 = m - 49, m - 25

    # Order matters: replace longer / more specific tokens first.
    reps = [
        # Direct-link batch list and markers (25791..25800 decade of template)
        ("graph_batch25791", f"graph_batch{d0}"),
        ("graph_batch25792", f"graph_batch{d0 + 1}"),
        ("graph_batch25793", f"graph_batch{d0 + 2}"),
        ("graph_batch25794", f"graph_batch{d0 + 3}"),
        ("graph_batch25795", f"graph_batch{d0 + 4}"),
        ("graph_batch25796", f"graph_batch{d0 + 5}"),
        ("graph_batch25797", f"graph_batch{d0 + 6}"),
        ("graph_batch25798", f"graph_batch{d0 + 7}"),
        ("graph_batch25799", f"graph_batch{d0 + 8}"),
        ("graph_batch25800", f"graph_batch{m}"),
        ("__libcgj_batch25791", f"__libcgj_batch{d0}"),
        ("__libcgj_batch25792", f"__libcgj_batch{d0 + 1}"),
        ("__libcgj_batch25793", f"__libcgj_batch{d0 + 2}"),
        ("__libcgj_batch25794", f"__libcgj_batch{d0 + 3}"),
        ("__libcgj_batch25795", f"__libcgj_batch{d0 + 4}"),
        ("__libcgj_batch25796", f"__libcgj_batch{d0 + 5}"),
        ("__libcgj_batch25797", f"__libcgj_batch{d0 + 6}"),
        ("__libcgj_batch25798", f"__libcgj_batch{d0 + 7}"),
        ("__libcgj_batch25799", f"__libcgj_batch{d0 + 8}"),
        ("__libcgj_batch25800", f"__libcgj_batch{m}"),
        ("libcgj-batch25791", f"libcgj-batch{d0}"),
        ("libcgj-batch25792", f"libcgj-batch{d0 + 1}"),
        ("libcgj-batch25793", f"libcgj-batch{d0 + 2}"),
        ("libcgj-batch25794", f"libcgj-batch{d0 + 3}"),
        ("libcgj-batch25795", f"libcgj-batch{d0 + 4}"),
        ("libcgj-batch25796", f"libcgj-batch{d0 + 5}"),
        ("libcgj-batch25797", f"libcgj-batch{d0 + 6}"),
        ("libcgj-batch25798", f"libcgj-batch{d0 + 7}"),
        ("libcgj-batch25799", f"libcgj-batch{d0 + 8}"),
        ("libcgj-batch25800", f"libcgj-batch{m}"),
        # Wave / fill / identity ranges in prose
        ("25791–25800", f"{d0}–{m}"),
        ("25791-25800", f"{d0}-{m}"),
        ("25776–25790", f"{fill0}–{fill1}"),
        ("25776-25790", f"{fill0}-{fill1}"),
        ("25751–25775", f"{id0}–{id1}"),
        ("25751-25775", f"{id0}-{id1}"),
        ("graph_batch25751", f"graph_batch{id0}"),
        # Macro / path / symbol milestone id (after decade-specific swaps)
        ("25800", str(m)),
        ("m25800", f"m{m}"),
        ("M25800", f"M{m}"),
    ]
    out = text
    for a, b in reps:
        out = out.replace(a, b)
    return out


def main() -> None:
    c_tpl = SRC_C.read_text(encoding="utf-8")
    n_tpl = SRC_N.read_text(encoding="utf-8")
    created = []
    skipped = []
    for m in range(60100, 62001, 100):
        c_path = HERE / f"cgj_soft_milestone_{m}.c"
        n_path = HERE / f"NOTES_milestone_{m}.txt"
        for path, tpl in ((c_path, c_tpl), (n_path, n_tpl)):
            if path.exists():
                skipped.append(path.name)
                continue
            path.write_text(rewrite_from_25800(tpl, m), encoding="utf-8")
            created.append(path.name)
    print(f"created={len(created)} skipped={len(skipped)}")
    for name in created:
        print(f"  + {name}")
    for name in skipped:
        print(f"  = {name}")


if __name__ == "__main__":
    main()
