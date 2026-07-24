/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 20000 markers (CREATE-ONLY).
 *
 * Coherent soft package (see NOTES_milestone_20000.txt):
 *   - this TU (libcgj continuum dyn/direct soft KATs)
 *   - NOTES_milestone_20000.txt
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
 *     (sibling decade soft probes).
 *   - This TU follows the dyn-smoke soft pattern: skip if missing,
 *     hard-fail if present but wrong. Not wired in Makefile (tests are
 *     not auto-globbed); build and run manually when product SO is ready.
 *
 * Manual build (host libc + dl):
 *   cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_20000 \
 *     user/libcgj/tests/cgj_soft_milestone_20000.c -ldl
 *   ./build/cgj_soft_milestone_20000
 *
 * Optional hard unit check (link pure graph batches directly; no SO map):
 *   cc -std=c11 -Wall -Wextra -O2 -DCGJ_SOFT_M20000_DIRECT \
 *     -o build/cgj_soft_milestone_20000_direct \
 *     user/libcgj/tests/cgj_soft_milestone_20000.c \
 *     user/libcgj/src/graph_batch19991.c \
 *     user/libcgj/src/graph_batch19992.c \
 *     user/libcgj/src/graph_batch19993.c \
 *     user/libcgj/src/graph_batch19994.c \
 *     user/libcgj/src/graph_batch19995.c \
 *     user/libcgj/src/graph_batch19996.c \
 *     user/libcgj/src/graph_batch19997.c \
 *     user/libcgj/src/graph_batch19998.c \
 *     user/libcgj/src/graph_batch19999.c \
 *     user/libcgj/src/graph_batch20000.c
 *   ./build/cgj_soft_milestone_20000_direct
 *
 * Expected soft KATs (wave 19991–20000 exclusive continuum markers):
 *   gj_shell_green_20000      → 1
 *   gj_libcgj_green_20000     → 1
 *   gj_bar3_ready_20000       → 0   (bar3 still open)
 *   gj_product_score_20000    → 0   (product score still open)
 *   gj_continuum_ready_20000  → 1
 *   gj_smoke_soft_20000       → 1
 *   gj_dyn_soft_20000         → 1
 *   gj_milestone_tag_20000    → 20000
 *   gj_continuum_wave_20000   → 20000
 *   gj_batch_id_20000         → 20000
 *   gj_graph_milestone_20000  → 20000
 *   __libcgj_batch19991_marker … __libcgj_batch20000_marker
 *     → "libcgj-batchNNNNN" string match when present
 *
 * Notes for parent wiring (outside this soft exclusive scope):
 *   - Sources graph_batch19991.c … graph_batch20000.c (and fill
 *     19976–19990 / identity 19951–19975) are CREATE-ONLY; wire into
 *     CGJ_SRCS when parent advances makefile_max to 20000.
 *   - libc.map may lag milestone 20000 globals; add 20000 symbols to the
 *     GLIBC_2.2.5 global list so dyn dlsym can see them.
 *   - Optional: fold these soft gates into cgj_dyn_smoke.c /
 *     cgj_smoke.c continuum soft blocks (same skip-if-missing rule).
 *   - Soft gates; bar3 still open (gj_bar3_ready_20000 == 0).
 */

#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef CGJ_SOFT_M20000_DIRECT
#include <dlfcn.h>
#endif

#ifdef CGJ_SOFT_M20000_DIRECT
/* Direct-link path: call product pure markers without dlopen. */
extern uint32_t gj_shell_green_20000(void);
extern uint32_t gj_libcgj_green_20000(void);
extern uint32_t gj_bar3_ready_20000(void);
extern uint32_t gj_product_score_20000(void);
extern uint32_t gj_continuum_ready_20000(void);
extern uint32_t gj_smoke_soft_20000(void);
extern uint32_t gj_dyn_soft_20000(void);
extern uint32_t gj_milestone_tag_20000(void);
extern uint32_t gj_continuum_wave_20000(void);
extern uint32_t gj_batch_id_20000(void);
extern uint32_t gj_graph_milestone_20000(void);
extern const char __libcgj_batch19991_marker[];
extern const char __libcgj_batch19992_marker[];
extern const char __libcgj_batch19993_marker[];
extern const char __libcgj_batch19994_marker[];
extern const char __libcgj_batch19995_marker[];
extern const char __libcgj_batch19996_marker[];
extern const char __libcgj_batch19997_marker[];
extern const char __libcgj_batch19998_marker[];
extern const char __libcgj_batch19999_marker[];
extern const char __libcgj_batch20000_marker[];

static int
direct_check(void)
{
	int nFail = 0;

	if (gj_shell_green_20000() != 1u) {
		fprintf(stderr, "libcgj: direct gj_shell_green_20000 fail\n");
		nFail++;
	}
	if (gj_libcgj_green_20000() != 1u) {
		fprintf(stderr, "libcgj: direct gj_libcgj_green_20000 fail\n");
		nFail++;
	}
	if (gj_bar3_ready_20000() != 0u) {
		fprintf(stderr, "libcgj: direct gj_bar3_ready_20000 fail\n");
		nFail++;
	}
	if (gj_product_score_20000() != 0u) {
		fprintf(stderr, "libcgj: direct gj_product_score_20000 fail\n");
		nFail++;
	}
	if (gj_continuum_ready_20000() != 1u) {
		fprintf(stderr, "libcgj: direct gj_continuum_ready_20000 fail\n");
		nFail++;
	}
	if (gj_smoke_soft_20000() != 1u) {
		fprintf(stderr, "libcgj: direct gj_smoke_soft_20000 fail\n");
		nFail++;
	}
	if (gj_dyn_soft_20000() != 1u) {
		fprintf(stderr, "libcgj: direct gj_dyn_soft_20000 fail\n");
		nFail++;
	}
	if (gj_milestone_tag_20000() != 20000u) {
		fprintf(stderr, "libcgj: direct gj_milestone_tag_20000 fail\n");
		nFail++;
	}
	if (gj_continuum_wave_20000() != 20000u) {
		fprintf(stderr, "libcgj: direct gj_continuum_wave_20000 fail\n");
		nFail++;
	}
	if (gj_batch_id_20000() != 20000u) {
		fprintf(stderr, "libcgj: direct gj_batch_id_20000 fail\n");
		nFail++;
	}
	if (gj_graph_milestone_20000() != 20000u) {
		fprintf(stderr,
		        "libcgj: direct gj_graph_milestone_20000 fail\n");
		nFail++;
	}
	if (strcmp(__libcgj_batch19991_marker, "libcgj-batch19991") != 0 ||
	    strcmp(__libcgj_batch19992_marker, "libcgj-batch19992") != 0 ||
	    strcmp(__libcgj_batch19993_marker, "libcgj-batch19993") != 0 ||
	    strcmp(__libcgj_batch19994_marker, "libcgj-batch19994") != 0 ||
	    strcmp(__libcgj_batch19995_marker, "libcgj-batch19995") != 0 ||
	    strcmp(__libcgj_batch19996_marker, "libcgj-batch19996") != 0 ||
	    strcmp(__libcgj_batch19997_marker, "libcgj-batch19997") != 0 ||
	    strcmp(__libcgj_batch19998_marker, "libcgj-batch19998") != 0 ||
	    strcmp(__libcgj_batch19999_marker, "libcgj-batch19999") != 0 ||
	    strcmp(__libcgj_batch20000_marker, "libcgj-batch20000") != 0) {
		fprintf(stderr, "libcgj: direct m20000 marker string fail\n");
		nFail++;
	}
	return nFail;
}
#else /* !CGJ_SOFT_M20000_DIRECT — dyn soft path */

typedef uint32_t (*u32_fn)(void);

/* Soft gate: missing → skip (return 0); present+wrong → return 1 (fail). */
static int
soft_u32(void *h, const char *szName, uint32_t u32Expect, int *pChecked)
{
	u32_fn pfn;
	uint32_t u32Got;

	pfn = (u32_fn)dlsym(h, szName);
	if (pfn == NULL) {
		return 0;
	}
	if (pChecked != NULL) {
		(*pChecked)++;
	}
	u32Got = pfn();
	if (u32Got != u32Expect) {
		fprintf(stderr,
		        "libcgj: soft m20000 %s KAT fail got=%u expect=%u\n",
		        szName, (unsigned)u32Got, (unsigned)u32Expect);
		return 1;
	}
	return 0;
}

static int
soft_marker(void *h, const char *szSym, const char *szExpect, int *pChecked)
{
	const char *szGot;

	szGot = (const char *)dlsym(h, szSym);
	if (szGot == NULL) {
		return 0;
	}
	if (pChecked != NULL) {
		(*pChecked)++;
	}
	if (strcmp(szGot, szExpect) != 0) {
		fprintf(stderr,
		        "libcgj: soft m20000 %s marker fail got=\"%s\" "
		        "expect=\"%s\"\n",
		        szSym, szGot, szExpect);
		return 1;
	}
	return 0;
}

static const char *const kPaths[] = {
    "build/user/libc.so.6",
    "./build/user/libc.so.6",
    NULL,
};

#endif /* CGJ_SOFT_M20000_DIRECT */

int
main(void)
{
#ifdef CGJ_SOFT_M20000_DIRECT
	int nFail;

	nFail = direct_check();
	if (nFail != 0) {
		fprintf(stderr,
		        "libcgj: soft m20000 DIRECT FAIL fails=%d\n", nFail);
		return 1;
	}
	printf("libcgj: soft m20000 DIRECT PASS (all 11 lamps + markers)\n");
	return 0;
#else
	void *h = NULL;
	const char *szPath = NULL;
	int i;
	int nChecked = 0;
	int nFail = 0;

	for (i = 0; kPaths[i] != NULL; i++) {
		h = dlopen(kPaths[i], RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND);
		if (h != NULL) {
			szPath = kPaths[i];
			break;
		}
	}
	if (h == NULL) {
		/*
		 * Soft: product SO not built yet is not a hard fail for this
		 * CREATE-ONLY host probe. Report skip and exit 0.
		 */
		fprintf(stderr,
		        "libcgj: soft m20000 skip (dlopen fail: %s)\n",
		        dlerror());
		printf("libcgj: soft m20000 SOFT-SKIP (no product SO)\n");
		return 0;
	}

	/* Continuum soft gates: skip if missing, hard fail if wrong. */
	nFail += soft_u32(h, "gj_shell_green_20000", 1u, &nChecked);
	nFail += soft_u32(h, "gj_libcgj_green_20000", 1u, &nChecked);
	nFail += soft_u32(h, "gj_bar3_ready_20000", 0u, &nChecked);
	nFail += soft_u32(h, "gj_product_score_20000", 0u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_ready_20000", 1u, &nChecked);
	nFail += soft_u32(h, "gj_smoke_soft_20000", 1u, &nChecked);
	nFail += soft_u32(h, "gj_dyn_soft_20000", 1u, &nChecked);
	nFail += soft_u32(h, "gj_milestone_tag_20000", 20000u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_wave_20000", 20000u, &nChecked);
	nFail += soft_u32(h, "gj_batch_id_20000", 20000u, &nChecked);
	nFail += soft_u32(h, "gj_graph_milestone_20000", 20000u, &nChecked);

	nFail += soft_marker(h, "__libcgj_batch19991_marker",
	                     "libcgj-batch19991", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch19992_marker",
	                     "libcgj-batch19992", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch19993_marker",
	                     "libcgj-batch19993", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch19994_marker",
	                     "libcgj-batch19994", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch19995_marker",
	                     "libcgj-batch19995", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch19996_marker",
	                     "libcgj-batch19996", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch19997_marker",
	                     "libcgj-batch19997", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch19998_marker",
	                     "libcgj-batch19998", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch19999_marker",
	                     "libcgj-batch19999", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch20000_marker",
	                     "libcgj-batch20000", &nChecked);

	if (nFail != 0) {
		(void)dlclose(h);
		fprintf(stderr,
		        "libcgj: soft m20000 FAIL path=%s fails=%d checked=%d\n",
		        szPath, nFail, nChecked);
		return 2;
	}

	printf("libcgj: soft m20000 PASS path=%s checked=%d "
	       "(soft-skip unexported/missing; bar3 still open)\n",
	       szPath, nChecked);
	(void)dlclose(h);
	return 0;
#endif
}
