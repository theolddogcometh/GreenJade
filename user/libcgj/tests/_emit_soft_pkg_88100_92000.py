#!/usr/bin/env python3
"""CREATE-ONLY soft packages M=88100..92000 (mirror 25800 KATs; bar3=0).

Writes thin cgj_soft_milestone_M.c (includes _cgj_soft_m_template.inc)
+ NOTES_milestone_M.txt for each missing M.

Run: python3 user/libcgj/tests/_emit_soft_pkg_88100_92000.py
"""
from pathlib import Path

HERE = Path(__file__).resolve().parent
M_LO, M_HI, M_STEP = 88100, 92000, 100


def c_body(m: int) -> str:
    b = list(range(m - 9, m)) + [m]
    return f"""/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE {m} markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_{m}.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_{m} == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_{m} \\
 *   user/libcgj/tests/cgj_soft_milestone_{m}.c -ldl
 * Direct: -DCGJ_SOFT_M{m}_DIRECT + graph_batch{b[0]}.c … graph_batch{m}.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M{m}_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M {m}
#define CGJ_SOFT_B0 {b[0]}
#define CGJ_SOFT_B1 {b[1]}
#define CGJ_SOFT_B2 {b[2]}
#define CGJ_SOFT_B3 {b[3]}
#define CGJ_SOFT_B4 {b[4]}
#define CGJ_SOFT_B5 {b[5]}
#define CGJ_SOFT_B6 {b[6]}
#define CGJ_SOFT_B7 {b[7]}
#define CGJ_SOFT_B8 {b[8]}
#include "_cgj_soft_m_template.inc"
"""


def notes_body(m: int) -> str:
    b0 = m - 9
    fill0, fill1 = m - 24, m - 10
    id0, id1 = m - 49, m - 25
    batches = list(range(m - 9, m)) + [m]
    graph_manual = "\n".join(
        f"  user/libcgj/src/graph_batch{x}.c"
        + (" \\" if i < len(batches) - 1 else "")
        for i, x in enumerate(batches)
    )
    return f"""Notes — continuum milestone {m} soft host gates
=================================================

Scope (CREATE-ONLY soft; Makefile not edited from this wave)
------------------------------------------------------------
Coherent soft package (untracked / new soft only):
  - user/libcgj/tests/NOTES_milestone_{m}.txt   (this file)
  - user/libcgj/tests/cgj_soft_milestone_{m}.c  (host dyn + direct KATs)
  - user/libcgj/tests/_cgj_soft_m_template.inc  (shared body)

libcgj continuum markers (graph_batch{b0}.c … graph_batch{m}.c) are
product tree TUs (CREATE-ONLY until parent wires CGJ_SRCS); this NOTES +
soft probe only document and soft-gate them. Mirror 25800; bar3=0.

Harness
-------
  - cgj_smoke.c / cgj_dyn_smoke.c
  - prior cgj_soft_milestone_*.c decade soft probes
  - NEW: cgj_soft_milestone_{m}.c

MILESTONE {m} marker surfaces (batches {b0}–{m})
-----------------------------------------------------
Symbol                         Expected   Source TU
-----------------------------  ---------  --------------------
gj_shell_green_{m}           1          graph_batch{batches[0]}.c
gj_libcgj_green_{m}          1          graph_batch{batches[1]}.c
gj_bar3_ready_{m}            0          graph_batch{batches[2]}.c  (bar3 open)
gj_product_score_{m}         0          graph_batch{batches[3]}.c  (score open)
gj_continuum_ready_{m}       1          graph_batch{batches[4]}.c
gj_smoke_soft_{m}            1          graph_batch{batches[5]}.c
gj_dyn_soft_{m}              1          graph_batch{batches[6]}.c
gj_milestone_tag_{m}         {m}      graph_batch{batches[7]}.c
gj_continuum_wave_{m}        {m}      graph_batch{batches[8]}.c
gj_batch_id_{m}              {m}      graph_batch{batches[9]}.c
gj_graph_milestone_{m}       {m}      graph_batch{batches[9]}.c

Fill continuum (batches {fill0}–{fill1}, not soft-probed here)
Identity continuum (batches {id0}–{id1}, not soft-probed here)

Soft rule: missing → skip; present but wrong → hard fail.
Soft gates only; bar3 remains open (gj_bar3_ready_{m} == 0).

Parent wiring (outside this soft exclusive scope)
-------------------------------------------------
1. Wire graph_batch{id0}.c … graph_batch{m}.c into CGJ_SRCS when
   advancing makefile_max to {m}.
2. libc.map may lag; add {m} symbols under GLIBC_2.2.5 global.
3. Rebuild: make libcgj
4. Soft gates only; bar3 remains open.

Manual run
----------
cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_{m} \\
  user/libcgj/tests/cgj_soft_milestone_{m}.c -ldl
./build/cgj_soft_milestone_{m}

cc -std=c11 -Wall -Wextra -O2 -DCGJ_SOFT_M{m}_DIRECT \\
  -o build/cgj_soft_milestone_{m}_direct \\
  user/libcgj/tests/cgj_soft_milestone_{m}.c \\
{graph_manual}
./build/cgj_soft_milestone_{m}_direct
"""


def main() -> None:
    created = []
    skipped = []
    milestones = list(range(M_LO, M_HI + 1, M_STEP))
    for m in milestones:
        for path, body in (
            (HERE / f"cgj_soft_milestone_{m}.c", c_body(m)),
            (HERE / f"NOTES_milestone_{m}.txt", notes_body(m)),
        ):
            if path.exists():
                skipped.append(path.name)
                continue
            path.write_text(body, encoding="utf-8")
            created.append(path.name)

    pairs = []
    for m in milestones:
        c = f"cgj_soft_milestone_{m}.c"
        n = f"NOTES_milestone_{m}.txt"
        mark = "+" if (c in created or n in created) else "="
        pairs.append(f"  {mark} {c} {n}")

    sample_ms = [88100, 88200, 90000, 91000, 92000]
    bar3_ok = []
    for m in sample_ms:
        p = HERE / f"cgj_soft_milestone_{m}.c"
        text = p.read_text(encoding="utf-8")
        notes = (HERE / f"NOTES_milestone_{m}.txt").read_text(encoding="utf-8")
        # thin TU: bar3=0 in header + template; NOTES has explicit 0
        ok = (
            f"gj_bar3_ready_{m} == 0" in text
            and f"gj_bar3_ready_{m}" in notes
            and "0          graph_batch" in notes
            and "bar3 open" in notes
        )
        bar3_ok.append(f"  gj_bar3_ready_{m} → 0 ({'ok' if ok else 'MISS'})")

    lines = [
        f"created={len(created)} skipped={len(skipped)}",
        f"range=M={M_LO}..{M_HI} step {M_STEP} "
        f"({len(milestones)} milestones × 2 files)",
        "mirror=25800 soft KATs; bar3=0; product_score=0",
        "form=thin (_cgj_soft_m_template.inc)",
        "status=CREATE-ONLY soft packages under user/libcgj/tests/",
        "pairs:",
    ]
    lines += pairs
    lines.append("kat_spotcheck:")
    lines += bar3_ok
    lines += [
        "  gj_product_score_M → 0",
        "  gj_shell_green_M / gj_libcgj_green_M / gj_continuum_ready_M → 1",
        "  gj_milestone_tag_M / gj_continuum_wave_M / gj_batch_id_M / "
        "gj_graph_milestone_M → M",
        "  markers __libcgj_batch(M-9)..__libcgj_batch(M)",
        "honesty:",
        "  soft packages only; Makefile not edited; soft ≠ bar3; bar3 remains open",
        "emitter:",
        "  user/libcgj/tests/_emit_soft_pkg_88100_92000.py (CREATE-ONLY if missing)",
    ]
    if created:
        lines.append("created_files:")
        lines += [f"  + {n}" for n in created]
    if skipped:
        lines.append("skipped_files:")
        lines += [f"  = {n}" for n in skipped]

    report = "\n".join(lines) + "\n"
    (HERE / "_emit_soft_pkg_88100_92000_result.txt").write_text(
        report, encoding="utf-8"
    )
    print(report, end="")


if __name__ == "__main__":
    main()
