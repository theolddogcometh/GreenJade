#!/usr/bin/env python3
"""CREATE-ONLY soft packages M=76100..80000 (mirror 25800 KATs; bar3=0).

Writes thin cgj_soft_milestone_M.c (includes _cgj_soft_m_template.inc)
+ NOTES_milestone_M.txt for each missing M.
Skips existing files. Full-form packages already present (e.g. 76100-76400) are left.

Run: python3 user/libcgj/tests/_emit_soft_pkg_76100_80000.py
"""
from pathlib import Path

HERE = Path(__file__).resolve().parent


def c_body(m: int) -> str:
    b = list(range(m - 9, m)) + [m]
    b0 = b[0]
    fill0, fill1 = m - 24, m - 10
    id0, id1 = m - 49, m - 25
    gl = "\n".join(
        f" *     user/libcgj/src/graph_batch{x}.c"
        + (" \\" if i < len(b) - 1 else "")
        for i, x in enumerate(b)
    )
    return f"""/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE {m} markers (CREATE-ONLY).
 *
 * Coherent soft package (see NOTES_milestone_{m}.txt):
 *   - this TU (libcgj continuum dyn/direct soft KATs)
 *   - NOTES_milestone_{m}.txt
 *   - _cgj_soft_m_template.inc (shared body)
 *
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_{m} == 0).
 * Sibling harnesses: cgj_smoke.c / cgj_dyn_smoke.c / prior soft decades.
 * Not wired in Makefile; build and run manually when product SO is ready.
 *
 * Manual build (host libc + dl):
 *   cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_{m} \\
 *     user/libcgj/tests/cgj_soft_milestone_{m}.c -ldl
 *   ./build/cgj_soft_milestone_{m}
 *
 * Optional hard unit check (link pure graph batches directly; no SO map):
 *   cc -std=c11 -Wall -Wextra -O2 -DCGJ_SOFT_M{m}_DIRECT \\
 *     -o build/cgj_soft_milestone_{m}_direct \\
 *     user/libcgj/tests/cgj_soft_milestone_{m}.c \\
{gl}
 *   ./build/cgj_soft_milestone_{m}_direct
 *
 * Expected soft KATs (wave {b0}–{m}): gj_shell_green_{m}→1,
 * gj_libcgj_green_{m}→1, gj_bar3_ready_{m}→0, gj_product_score_{m}→0,
 * gj_continuum_ready_{m}→1, gj_smoke_soft_{m}→1, gj_dyn_soft_{m}→1,
 * gj_milestone_tag/wave/batch_id/graph_milestone_{m}→{m}, markers match.
 *
 * Parent wiring: graph_batch{b0}.c … graph_batch{m}.c (fill {fill0}–{fill1},
 * identity {id0}–{id1}) CREATE-ONLY until makefile_max advances to {m}.
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
    for m in range(76100, 80001, 100):
        for path, body in (
            (HERE / f"cgj_soft_milestone_{m}.c", c_body(m)),
            (HERE / f"NOTES_milestone_{m}.txt", notes_body(m)),
        ):
            if path.exists():
                skipped.append(path.name)
                continue
            path.write_text(body, encoding="utf-8")
            created.append(path.name)
    lines = [f"created={len(created)} skipped={len(skipped)}"]
    lines += [f"  + {n}" for n in created]
    if skipped:
        lines.append("skipped:")
        lines += [f"  = {n}" for n in skipped]
    report = "\n".join(lines) + "\n"
    (HERE / "_emit_soft_pkg_76100_80000_result.txt").write_text(
        report, encoding="utf-8"
    )
    print(report, end="")


if __name__ == "__main__":
    main()
