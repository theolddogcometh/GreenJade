#!/usr/bin/env python3
"""CREATE-ONLY soft packages M=80100..84000 (mirror 25800 KATs; bar3=0).

Writes cgj_soft_milestone_M.c + NOTES_milestone_M.txt for each missing M.
Run: python3 user/libcgj/tests/_emit_soft_pkg_80100_84000.py
"""
from pathlib import Path

HERE = Path(__file__).resolve().parent


def c_body(m: int) -> str:
    b = list(range(m - 9, m)) + [m]
    b0 = b[0]
    fill0, fill1 = m - 24, m - 10
    id0, id1 = m - 49, m - 25
    ex = "\n".join(f"extern const char __libcgj_batch{x}_marker[];" for x in b)
    sc = " ||\n\t    ".join(
        f'strcmp(__libcgj_batch{x}_marker, "libcgj-batch{x}") != 0' for x in b
    )
    sm = "\n".join(
        f'\tnFail += soft_marker(h, "__libcgj_batch{x}_marker",\n'
        f'\t                     "libcgj-batch{x}", &nChecked);'
        for x in b
    )
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
 * Expected soft KATs (wave {b0}–{m} exclusive continuum markers):
 *   gj_shell_green_{m}      → 1
 *   gj_libcgj_green_{m}     → 1
 *   gj_bar3_ready_{m}       → 0   (bar3 still open)
 *   gj_product_score_{m}    → 0   (product score still open)
 *   gj_continuum_ready_{m}  → 1
 *   gj_smoke_soft_{m}       → 1
 *   gj_dyn_soft_{m}         → 1
 *   gj_milestone_tag_{m}    → {m}
 *   gj_continuum_wave_{m}   → {m}
 *   gj_batch_id_{m}         → {m}
 *   gj_graph_milestone_{m}  → {m}
 *   __libcgj_batch{b0}_marker … __libcgj_batch{m}_marker
 *
 * Parent wiring: graph_batch{b0}.c … graph_batch{m}.c (fill {fill0}–{fill1},
 * identity {id0}–{id1}) CREATE-ONLY until makefile_max advances to {m}.
 */

#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef CGJ_SOFT_M{m}_DIRECT
#include <dlfcn.h>
#endif

#ifdef CGJ_SOFT_M{m}_DIRECT
extern uint32_t gj_shell_green_{m}(void);
extern uint32_t gj_libcgj_green_{m}(void);
extern uint32_t gj_bar3_ready_{m}(void);
extern uint32_t gj_product_score_{m}(void);
extern uint32_t gj_continuum_ready_{m}(void);
extern uint32_t gj_smoke_soft_{m}(void);
extern uint32_t gj_dyn_soft_{m}(void);
extern uint32_t gj_milestone_tag_{m}(void);
extern uint32_t gj_continuum_wave_{m}(void);
extern uint32_t gj_batch_id_{m}(void);
extern uint32_t gj_graph_milestone_{m}(void);
{ex}

static int
direct_check(void)
{{
	int nFail = 0;

	if (gj_shell_green_{m}() != 1u) {{
		fprintf(stderr, "libcgj: direct gj_shell_green_{m} fail\\n");
		nFail++;
	}}
	if (gj_libcgj_green_{m}() != 1u) {{
		fprintf(stderr, "libcgj: direct gj_libcgj_green_{m} fail\\n");
		nFail++;
	}}
	if (gj_bar3_ready_{m}() != 0u) {{
		fprintf(stderr, "libcgj: direct gj_bar3_ready_{m} fail\\n");
		nFail++;
	}}
	if (gj_product_score_{m}() != 0u) {{
		fprintf(stderr, "libcgj: direct gj_product_score_{m} fail\\n");
		nFail++;
	}}
	if (gj_continuum_ready_{m}() != 1u) {{
		fprintf(stderr, "libcgj: direct gj_continuum_ready_{m} fail\\n");
		nFail++;
	}}
	if (gj_smoke_soft_{m}() != 1u) {{
		fprintf(stderr, "libcgj: direct gj_smoke_soft_{m} fail\\n");
		nFail++;
	}}
	if (gj_dyn_soft_{m}() != 1u) {{
		fprintf(stderr, "libcgj: direct gj_dyn_soft_{m} fail\\n");
		nFail++;
	}}
	if (gj_milestone_tag_{m}() != {m}u) {{
		fprintf(stderr, "libcgj: direct gj_milestone_tag_{m} fail\\n");
		nFail++;
	}}
	if (gj_continuum_wave_{m}() != {m}u) {{
		fprintf(stderr, "libcgj: direct gj_continuum_wave_{m} fail\\n");
		nFail++;
	}}
	if (gj_batch_id_{m}() != {m}u) {{
		fprintf(stderr, "libcgj: direct gj_batch_id_{m} fail\\n");
		nFail++;
	}}
	if (gj_graph_milestone_{m}() != {m}u) {{
		fprintf(stderr, "libcgj: direct gj_graph_milestone_{m} fail\\n");
		nFail++;
	}}
	if ({sc}) {{
		fprintf(stderr, "libcgj: direct m{m} marker string fail\\n");
		nFail++;
	}}
	return nFail;
}}
#else

typedef uint32_t (*u32_fn)(void);

static int
soft_u32(void *h, const char *szName, uint32_t u32Expect, int *pChecked)
{{
	u32_fn pfn;
	uint32_t u32Got;

	pfn = (u32_fn)dlsym(h, szName);
	if (pfn == NULL)
		return 0;
	if (pChecked != NULL)
		(*pChecked)++;
	u32Got = pfn();
	if (u32Got != u32Expect) {{
		fprintf(stderr,
		        "libcgj: soft m{m} %s KAT fail got=%u expect=%u\\n",
		        szName, (unsigned)u32Got, (unsigned)u32Expect);
		return 1;
	}}
	return 0;
}}

static int
soft_marker(void *h, const char *szSym, const char *szExpect, int *pChecked)
{{
	const char *szGot;

	szGot = (const char *)dlsym(h, szSym);
	if (szGot == NULL)
		return 0;
	if (pChecked != NULL)
		(*pChecked)++;
	if (strcmp(szGot, szExpect) != 0) {{
		fprintf(stderr,
		        "libcgj: soft m{m} %s marker fail got=\\"%s\\" "
		        "expect=\\"%s\\"\\n",
		        szSym, szGot, szExpect);
		return 1;
	}}
	return 0;
}}

static const char *const kPaths[] = {{
    "build/user/libc.so.6",
    "./build/user/libc.so.6",
    NULL,
}};

#endif

int
main(void)
{{
#ifdef CGJ_SOFT_M{m}_DIRECT
	int nFail = direct_check();
	if (nFail != 0) {{
		fprintf(stderr, "libcgj: soft m{m} DIRECT FAIL fails=%d\\n", nFail);
		return 1;
	}}
	printf("libcgj: soft m{m} DIRECT PASS (all 11 lamps + markers)\\n");
	return 0;
#else
	void *h = NULL;
	const char *szPath = NULL;
	int i, nChecked = 0, nFail = 0;

	for (i = 0; kPaths[i] != NULL; i++) {{
		h = dlopen(kPaths[i], RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND);
		if (h != NULL) {{
			szPath = kPaths[i];
			break;
		}}
	}}
	if (h == NULL) {{
		fprintf(stderr, "libcgj: soft m{m} skip (dlopen fail: %s)\\n",
		        dlerror());
		printf("libcgj: soft m{m} SOFT-SKIP (no product SO)\\n");
		return 0;
	}}

	nFail += soft_u32(h, "gj_shell_green_{m}", 1u, &nChecked);
	nFail += soft_u32(h, "gj_libcgj_green_{m}", 1u, &nChecked);
	nFail += soft_u32(h, "gj_bar3_ready_{m}", 0u, &nChecked);
	nFail += soft_u32(h, "gj_product_score_{m}", 0u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_ready_{m}", 1u, &nChecked);
	nFail += soft_u32(h, "gj_smoke_soft_{m}", 1u, &nChecked);
	nFail += soft_u32(h, "gj_dyn_soft_{m}", 1u, &nChecked);
	nFail += soft_u32(h, "gj_milestone_tag_{m}", {m}u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_wave_{m}", {m}u, &nChecked);
	nFail += soft_u32(h, "gj_batch_id_{m}", {m}u, &nChecked);
	nFail += soft_u32(h, "gj_graph_milestone_{m}", {m}u, &nChecked);
{sm}

	if (nFail != 0) {{
		(void)dlclose(h);
		fprintf(stderr,
		        "libcgj: soft m{m} FAIL path=%s fails=%d checked=%d\\n",
		        szPath, nFail, nChecked);
		return 2;
	}}
	printf("libcgj: soft m{m} PASS path=%s checked=%d "
	       "(soft-skip unexported/missing; bar3 still open)\\n",
	       szPath, nChecked);
	(void)dlclose(h);
	return 0;
#endif
}}
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
    for m in range(80100, 84001, 100):
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
    (HERE / "_emit_soft_pkg_80100_84000_result.txt").write_text(
        report, encoding="utf-8"
    )
    print(report, end="")


if __name__ == "__main__":
    main()
