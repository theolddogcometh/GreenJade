#!/usr/bin/env python3
"""Emit GreenJade soft packages M=68100..70000 (mirror 25800; bar3=0).

CREATE-ONLY: skips existing packages (both .c and NOTES present).
Run: python3 user/libcgj/tests/_emit_soft_pkg_68100_70000.py
"""
from __future__ import annotations

from pathlib import Path

HERE = Path(__file__).resolve().parent


def gen_c(m: int) -> str:
    d = [m - 9 + i for i in range(9)] + [m]  # exclusive decade batches
    fill0, fill1 = m - 24, m - 10
    id0, id1 = m - 49, m - 25
    b = d  # alias
    graph_lines = []
    for i, x in enumerate(b):
        end = " \\" if i < len(b) - 1 else ""
        graph_lines.append(f" *     user/libcgj/src/graph_batch{x}.c{end}")
    graph_block = "\n".join(graph_lines)

    extern_markers = "\n".join(
        f"extern const char __libcgj_batch{x}_marker[];" for x in b
    )
    strcmp_chain = " ||\n\t    ".join(
        f'strcmp(__libcgj_batch{x}_marker, "libcgj-batch{x}") != 0' for x in b
    )
    soft_markers = "\n".join(
        f'\tnFail += soft_marker(h, "__libcgj_batch{x}_marker",\n'
        f'\t                     "libcgj-batch{x}", &nChecked);'
        for x in b
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
 * Test harness note:
 *   - Sibling harnesses: cgj_smoke.c (static freestanding weak),
 *     cgj_dyn_smoke.c (host dlopen product libc.so.6),
 *     cgj_soft_milestone_14500.c / cgj_soft_milestone_14600.c /
 *     cgj_soft_milestone_14700.c / cgj_soft_milestone_14800.c /
 *     cgj_soft_milestone_14900.c / cgj_soft_milestone_15000.c /
 *     cgj_soft_milestone_15100.c / cgj_soft_milestone_15200.c /
 *     cgj_soft_milestone_15300.c / cgj_soft_milestone_15400.c /
 *     cgj_soft_milestone_15500.c / cgj_soft_milestone_15600.c /
 *     cgj_soft_milestone_15700.c / cgj_soft_milestone_15800.c /
 *     cgj_soft_milestone_15900.c / cgj_soft_milestone_16000.c /
 *     cgj_soft_milestone_16100.c / cgj_soft_milestone_16200.c /
 *     cgj_soft_milestone_16300.c / cgj_soft_milestone_16400.c /
 *     cgj_soft_milestone_16500.c / cgj_soft_milestone_16600.c /
 *     cgj_soft_milestone_16700.c / cgj_soft_milestone_16800.c /
 *     cgj_soft_milestone_16900.c / cgj_soft_milestone_17000.c /
 *     cgj_soft_milestone_17100.c / cgj_soft_milestone_17200.c /
 *     cgj_soft_milestone_17300.c / cgj_soft_milestone_17400.c /
 *     cgj_soft_milestone_17500.c / cgj_soft_milestone_17600.c /
 *     cgj_soft_milestone_17700.c / cgj_soft_milestone_17800.c /
 *     cgj_soft_milestone_17900.c / cgj_soft_milestone_18000.c /
 *     cgj_soft_milestone_18100.c / cgj_soft_milestone_18200.c /
 *     cgj_soft_milestone_18300.c / cgj_soft_milestone_18400.c /
 *     cgj_soft_milestone_18500.c / cgj_soft_milestone_18600.c /
 *     cgj_soft_milestone_18700.c / cgj_soft_milestone_18800.c /
 *     cgj_soft_milestone_18900.c / cgj_soft_milestone_19000.c /
 *     cgj_soft_milestone_19100.c / cgj_soft_milestone_19200.c /
 *     cgj_soft_milestone_19300.c / cgj_soft_milestone_19400.c /
 *     cgj_soft_milestone_19500.c / cgj_soft_milestone_19600.c /
 *     cgj_soft_milestone_19700.c / cgj_soft_milestone_19800.c /
 *     cgj_soft_milestone_19900.c / cgj_soft_milestone_20000.c /
 *     cgj_soft_milestone_20100.c / cgj_soft_milestone_20200.c /
 *     cgj_soft_milestone_20300.c / cgj_soft_milestone_20400.c /
 *     cgj_soft_milestone_20500.c / cgj_soft_milestone_20600.c /
 *     cgj_soft_milestone_20700.c / cgj_soft_milestone_20800.c /
 *     cgj_soft_milestone_20900.c / cgj_soft_milestone_21000.c /
 *     cgj_soft_milestone_21100.c / cgj_soft_milestone_21200.c /
 *     cgj_soft_milestone_21300.c / cgj_soft_milestone_21400.c /
 *     cgj_soft_milestone_21500.c / cgj_soft_milestone_21600.c /
 *     cgj_soft_milestone_21700.c / cgj_soft_milestone_21800.c /
 *     cgj_soft_milestone_21900.c / cgj_soft_milestone_22000.c /
 *     cgj_soft_milestone_22100.c / cgj_soft_milestone_22200.c /
 *     cgj_soft_milestone_22300.c / cgj_soft_milestone_22400.c /
 *     cgj_soft_milestone_22500.c / cgj_soft_milestone_22600.c /
 *     cgj_soft_milestone_22700.c / cgj_soft_milestone_22800.c /
 *     cgj_soft_milestone_22900.c / cgj_soft_milestone_23000.c /
 *     cgj_soft_milestone_23100.c / cgj_soft_milestone_23200.c /
 *     cgj_soft_milestone_23300.c / cgj_soft_milestone_23400.c /
 *     cgj_soft_milestone_23500.c / cgj_soft_milestone_23600.c /
 *     cgj_soft_milestone_23700.c / cgj_soft_milestone_23800.c /
 *     cgj_soft_milestone_23900.c / cgj_soft_milestone_24000.c /
 *     cgj_soft_milestone_24100.c / cgj_soft_milestone_24200.c /
 *     cgj_soft_milestone_24300.c / cgj_soft_milestone_24400.c /
 *     cgj_soft_milestone_24500.c / cgj_soft_milestone_24600.c /
 *     cgj_soft_milestone_24700.c / cgj_soft_milestone_24800.c /
 *     cgj_soft_milestone_24900.c / cgj_soft_milestone_25000.c /
 *     cgj_soft_milestone_25100.c /
 *     cgj_soft_milestone_25200.c / cgj_soft_milestone_25300.c /
 *     cgj_soft_milestone_25400.c / cgj_soft_milestone_25500.c /
 *     cgj_soft_milestone_25600.c / cgj_soft_milestone_25700.c /
 *     (sibling decade soft probes).
 *   - This TU follows the dyn-smoke soft pattern: skip if missing,
 *     hard-fail if present but wrong. Not wired in Makefile (tests are
 *     not auto-globbed); build and run manually when product SO is ready.
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
{graph_block}
 *   ./build/cgj_soft_milestone_{m}_direct
 *
 * Expected soft KATs (wave {b[0]}–{m} exclusive continuum markers):
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
 *   __libcgj_batch{b[0]}_marker … __libcgj_batch{m}_marker
 *     → "libcgj-batchNNNNN" string match when present
 *
 * Notes for parent wiring (outside this soft exclusive scope):
 *   - Sources graph_batch{b[0]}.c … graph_batch{m}.c (and fill
 *     {fill0}–{fill1} / identity {id0}–{id1}) are CREATE-ONLY; wire into
 *     CGJ_SRCS when parent advances makefile_max to {m}.
 *   - libc.map may lag milestone {m} globals; add {m} symbols to the
 *     GLIBC_2.2.5 global list so dyn dlsym can see them.
 *   - Optional: fold these soft gates into cgj_dyn_smoke.c /
 *     cgj_smoke.c continuum soft blocks (same skip-if-missing rule).
 *   - Soft gates; bar3 still open (gj_bar3_ready_{m} == 0).
 */

#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef CGJ_SOFT_M{m}_DIRECT
#include <dlfcn.h>
#endif

#ifdef CGJ_SOFT_M{m}_DIRECT
/* Direct-link path: call product pure markers without dlopen. */
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
{extern_markers}

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
		fprintf(stderr,
		        "libcgj: direct gj_graph_milestone_{m} fail\\n");
		nFail++;
	}}
	if ({strcmp_chain}) {{
		fprintf(stderr, "libcgj: direct m{m} marker string fail\\n");
		nFail++;
	}}
	return nFail;
}}
#else /* !CGJ_SOFT_M{m}_DIRECT — dyn soft path */

typedef uint32_t (*u32_fn)(void);

/* Soft gate: missing → skip (return 0); present+wrong → return 1 (fail). */
static int
soft_u32(void *h, const char *szName, uint32_t u32Expect, int *pChecked)
{{
	u32_fn pfn;
	uint32_t u32Got;

	pfn = (u32_fn)dlsym(h, szName);
	if (pfn == NULL) {{
		return 0;
	}}
	if (pChecked != NULL) {{
		(*pChecked)++;
	}}
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
	if (szGot == NULL) {{
		return 0;
	}}
	if (pChecked != NULL) {{
		(*pChecked)++;
	}}
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

#endif /* CGJ_SOFT_M{m}_DIRECT */

int
main(void)
{{
#ifdef CGJ_SOFT_M{m}_DIRECT
	int nFail;

	nFail = direct_check();
	if (nFail != 0) {{
		fprintf(stderr,
		        "libcgj: soft m{m} DIRECT FAIL fails=%d\\n", nFail);
		return 1;
	}}
	printf("libcgj: soft m{m} DIRECT PASS (all 11 lamps + markers)\\n");
	return 0;
#else
	void *h = NULL;
	const char *szPath = NULL;
	int i;
	int nChecked = 0;
	int nFail = 0;

	for (i = 0; kPaths[i] != NULL; i++) {{
		h = dlopen(kPaths[i], RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND);
		if (h != NULL) {{
			szPath = kPaths[i];
			break;
		}}
	}}
	if (h == NULL) {{
		/*
		 * Soft: product SO not built yet is not a hard fail for this
		 * CREATE-ONLY host probe. Report skip and exit 0.
		 */
		fprintf(stderr,
		        "libcgj: soft m{m} skip (dlopen fail: %s)\\n",
		        dlerror());
		printf("libcgj: soft m{m} SOFT-SKIP (no product SO)\\n");
		return 0;
	}}

	/* Continuum soft gates: skip if missing, hard fail if wrong. */
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

{soft_markers}

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


def gen_notes(m: int) -> str:
    b0 = m - 9
    fill0, fill1 = m - 24, m - 10
    id0, id1 = m - 49, m - 25
    batches = [m - 9 + i for i in range(9)] + [m]
    lines = []
    for i, x in enumerate(batches):
        lines.append(
            f"  user/libcgj/src/graph_batch{x}.c"
            + (" \\" if i < len(batches) - 1 else "")
        )
    graph_manual = "\n".join(lines)

    prior = "\n".join(
        f"  - cgj_soft_milestone_{n}.c  prior decade soft probe"
        for n in range(14500, 25701, 100)
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
soft probe only document and soft-gate them.

Harness
-------
Existing host harness:
  - cgj_smoke.c                 static freestanding + weak continuum soft gates
  - cgj_dyn_smoke.c             host dlopen product libc.so.6 soft KATs
{prior}

New CREATE-ONLY soft host probe:
  - cgj_soft_milestone_{m}.c

Not auto-picked by Makefile (explicit CGJ_SMOKE / CGJ_DYN_SMOKE targets
only). Build manually as documented in the .c file header.

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
__libcgj_batch20NNN_marker     string     matching batch TU

Fill continuum (batches {fill0}–{fill1}, not soft-probed here)
---------------------------------------------------------
Each TU exports unique gj_batch_id_N plus a small unique soft unit
helper (wave_fill / slot_a / … / anchor). Soft rule for this NOTES
file remains decade markers {b0}–{m} only.

Identity continuum (batches {id0}–{id1}, not soft-probed here)
-------------------------------------------------------------
Each TU exports unique gj_batch_id_N + gj_graph_milestone_N.

Soft rule: missing → skip; present but wrong → hard fail.

Parent wiring (outside this soft exclusive scope)
-------------------------------------------------
1. Wire graph_batch{id0}.c … graph_batch{m}.c into CGJ_SRCS when
   advancing makefile_max to {m} (CREATE-ONLY wave lands first).
2. libc.map may lag milestone {m} globals; add {m} symbols under
   GLIBC_2.2.5 global so dyn dlsym can resolve them (host dyn soft
   currently skip-if-missing when map/export lags).
3. Rebuild: make libcgj  (or product SO + archive).
4. Optional fold into cgj_dyn_smoke.c / cgj_smoke.c continuum soft
   blocks once map exports are present (host smokes deepened carefully
   with skip-if-missing decade lamps).
5. Soft gates only; bar3 remains open (gj_bar3_ready_{m} == 0).

Manual run
----------
# Dyn soft (product libc.so.6; soft-skip if symbols not exported yet):
cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_{m} \\
  user/libcgj/tests/cgj_soft_milestone_{m}.c -ldl
./build/cgj_soft_milestone_{m}

# Direct hard KAT (link pure continuum TUs; no version map needed):
cc -std=c11 -Wall -Wextra -O2 -DCGJ_SOFT_M{m}_DIRECT \\
  -o build/cgj_soft_milestone_{m}_direct \\
  user/libcgj/tests/cgj_soft_milestone_{m}.c \\
{graph_manual}
./build/cgj_soft_milestone_{m}_direct
"""


def main() -> None:
    created: list[str] = []
    skipped: list[str] = []
    for m in range(68100, 70001, 100):
        c_path = HERE / f"cgj_soft_milestone_{m}.c"
        n_path = HERE / f"NOTES_milestone_{m}.txt"
        for path, body in ((c_path, gen_c(m)), (n_path, gen_notes(m))):
            if path.exists():
                skipped.append(path.name)
                continue
            path.write_text(body, encoding="utf-8")
            created.append(path.name)
    print(f"created={len(created)} skipped={len(skipped)}")
    for name in created:
        print(f"  + {name}")
    if skipped:
        print("skipped:")
        for name in skipped:
            print(f"  = {name}")


if __name__ == "__main__":
    main()
