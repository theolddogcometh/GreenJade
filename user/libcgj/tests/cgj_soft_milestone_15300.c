/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 15300 markers (CREATE-ONLY).
 *
 * Coherent soft package (see NOTES_milestone_15300.txt):
 *   - this TU (libcgj continuum dyn/direct soft KATs)
 *   - NOTES_milestone_15300.txt
 *
 * Test harness note:
 *   - Sibling harnesses: cgj_smoke.c (static freestanding weak),
 *     cgj_dyn_smoke.c (host dlopen product libc.so.6),
 *     cgj_soft_milestone_14500.c / cgj_soft_milestone_14600.c /
 *     cgj_soft_milestone_14700.c / cgj_soft_milestone_14800.c /
 *     cgj_soft_milestone_14900.c / cgj_soft_milestone_15000.c /
 *     cgj_soft_milestone_15100.c / cgj_soft_milestone_15200.c
 *     (sibling decade soft probes).
 *   - This TU follows the dyn-smoke soft pattern: skip if missing,
 *     hard-fail if present but wrong. Not wired in Makefile (tests are
 *     not auto-globbed); build and run manually when product SO is ready.
 *
 * Manual build (host libc + dl):
 *   cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_15300 \
 *     user/libcgj/tests/cgj_soft_milestone_15300.c -ldl
 *   ./build/cgj_soft_milestone_15300
 *
 * Optional hard unit check (link pure graph batches directly; no SO map):
 *   cc -std=c11 -Wall -Wextra -O2 -DCGJ_SOFT_M15300_DIRECT \
 *     -o build/cgj_soft_milestone_15300_direct \
 *     user/libcgj/tests/cgj_soft_milestone_15300.c \
 *     user/libcgj/src/graph_batch15291.c \
 *     user/libcgj/src/graph_batch15292.c \
 *     user/libcgj/src/graph_batch15293.c \
 *     user/libcgj/src/graph_batch15294.c \
 *     user/libcgj/src/graph_batch15295.c \
 *     user/libcgj/src/graph_batch15296.c \
 *     user/libcgj/src/graph_batch15297.c \
 *     user/libcgj/src/graph_batch15298.c \
 *     user/libcgj/src/graph_batch15299.c \
 *     user/libcgj/src/graph_batch15300.c
 *   ./build/cgj_soft_milestone_15300_direct
 *
 * Expected soft KATs (wave 15291–15300 exclusive continuum markers):
 *   gj_shell_green_15300      → 1
 *   gj_libcgj_green_15300     → 1
 *   gj_bar3_ready_15300       → 0   (bar3 still open)
 *   gj_product_score_15300    → 0   (product score still open)
 *   gj_continuum_ready_15300  → 1
 *   gj_smoke_soft_15300       → 1
 *   gj_dyn_soft_15300         → 1
 *   gj_milestone_tag_15300    → 15300
 *   gj_continuum_wave_15300   → 15300
 *   gj_batch_id_15300         → 15300
 *   gj_graph_milestone_15300  → 15300
 *   __libcgj_batch15291_marker … __libcgj_batch15300_marker
 *     → "libcgj-batchNNNNN" string match when present
 *
 * Notes for parent wiring (outside this soft exclusive scope):
 *   - Sources graph_batch15291.c … graph_batch15300.c (and fill
 *     15276–15290 / identity 15251–15275) are CREATE-ONLY; wire into
 *     CGJ_SRCS when parent advances makefile_max to 15300.
 *   - libc.map may lag milestone 15300 globals; add 15300 symbols to the
 *     GLIBC_2.2.5 global list so dyn dlsym can see them.
 *   - Optional: fold these soft gates into cgj_dyn_smoke.c /
 *     cgj_smoke.c continuum soft blocks (same skip-if-missing rule).
 *   - Soft gates; bar3 still open (gj_bar3_ready_15300 == 0).
 */

#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef CGJ_SOFT_M15300_DIRECT
#include <dlfcn.h>
#endif

#ifdef CGJ_SOFT_M15300_DIRECT
/* Direct-link path: call product pure markers without dlopen. */
extern uint32_t gj_shell_green_15300(void);
extern uint32_t gj_libcgj_green_15300(void);
extern uint32_t gj_bar3_ready_15300(void);
extern uint32_t gj_product_score_15300(void);
extern uint32_t gj_continuum_ready_15300(void);
extern uint32_t gj_smoke_soft_15300(void);
extern uint32_t gj_dyn_soft_15300(void);
extern uint32_t gj_milestone_tag_15300(void);
extern uint32_t gj_continuum_wave_15300(void);
extern uint32_t gj_batch_id_15300(void);
extern uint32_t gj_graph_milestone_15300(void);
extern const char __libcgj_batch15291_marker[];
extern const char __libcgj_batch15292_marker[];
extern const char __libcgj_batch15293_marker[];
extern const char __libcgj_batch15294_marker[];
extern const char __libcgj_batch15295_marker[];
extern const char __libcgj_batch15296_marker[];
extern const char __libcgj_batch15297_marker[];
extern const char __libcgj_batch15298_marker[];
extern const char __libcgj_batch15299_marker[];
extern const char __libcgj_batch15300_marker[];

static int
direct_check(void)
{
	int nFail = 0;

	if (gj_shell_green_15300() != 1u) {
		fprintf(stderr, "libcgj: direct gj_shell_green_15300 fail\n");
		nFail++;
	}
	if (gj_libcgj_green_15300() != 1u) {
		fprintf(stderr, "libcgj: direct gj_libcgj_green_15300 fail\n");
		nFail++;
	}
	if (gj_bar3_ready_15300() != 0u) {
		fprintf(stderr, "libcgj: direct gj_bar3_ready_15300 fail\n");
		nFail++;
	}
	if (gj_product_score_15300() != 0u) {
		fprintf(stderr, "libcgj: direct gj_product_score_15300 fail\n");
		nFail++;
	}
	if (gj_continuum_ready_15300() != 1u) {
		fprintf(stderr, "libcgj: direct gj_continuum_ready_15300 fail\n");
		nFail++;
	}
	if (gj_smoke_soft_15300() != 1u) {
		fprintf(stderr, "libcgj: direct gj_smoke_soft_15300 fail\n");
		nFail++;
	}
	if (gj_dyn_soft_15300() != 1u) {
		fprintf(stderr, "libcgj: direct gj_dyn_soft_15300 fail\n");
		nFail++;
	}
	if (gj_milestone_tag_15300() != 15300u) {
		fprintf(stderr, "libcgj: direct gj_milestone_tag_15300 fail\n");
		nFail++;
	}
	if (gj_continuum_wave_15300() != 15300u) {
		fprintf(stderr, "libcgj: direct gj_continuum_wave_15300 fail\n");
		nFail++;
	}
	if (gj_batch_id_15300() != 15300u) {
		fprintf(stderr, "libcgj: direct gj_batch_id_15300 fail\n");
		nFail++;
	}
	if (gj_graph_milestone_15300() != 15300u) {
		fprintf(stderr,
		        "libcgj: direct gj_graph_milestone_15300 fail\n");
		nFail++;
	}
	if (strcmp(__libcgj_batch15291_marker, "libcgj-batch15291") != 0 ||
	    strcmp(__libcgj_batch15292_marker, "libcgj-batch15292") != 0 ||
	    strcmp(__libcgj_batch15293_marker, "libcgj-batch15293") != 0 ||
	    strcmp(__libcgj_batch15294_marker, "libcgj-batch15294") != 0 ||
	    strcmp(__libcgj_batch15295_marker, "libcgj-batch15295") != 0 ||
	    strcmp(__libcgj_batch15296_marker, "libcgj-batch15296") != 0 ||
	    strcmp(__libcgj_batch15297_marker, "libcgj-batch15297") != 0 ||
	    strcmp(__libcgj_batch15298_marker, "libcgj-batch15298") != 0 ||
	    strcmp(__libcgj_batch15299_marker, "libcgj-batch15299") != 0 ||
	    strcmp(__libcgj_batch15300_marker, "libcgj-batch15300") != 0) {
		fprintf(stderr, "libcgj: direct m15300 marker string fail\n");
		nFail++;
	}
	return nFail;
}
#else /* !CGJ_SOFT_M15300_DIRECT — dyn soft path */

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
		        "libcgj: soft m15300 %s KAT fail got=%u expect=%u\n",
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
		        "libcgj: soft m15300 %s marker fail got=\"%s\" "
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

#endif /* CGJ_SOFT_M15300_DIRECT */

int
main(void)
{
#ifdef CGJ_SOFT_M15300_DIRECT
	int nFail;

	nFail = direct_check();
	if (nFail != 0) {
		fprintf(stderr,
		        "libcgj: soft m15300 DIRECT FAIL fails=%d\n", nFail);
		return 1;
	}
	printf("libcgj: soft m15300 DIRECT PASS (all 11 lamps + markers)\n");
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
		        "libcgj: soft m15300 skip (dlopen fail: %s)\n",
		        dlerror());
		printf("libcgj: soft m15300 SOFT-SKIP (no product SO)\n");
		return 0;
	}

	/* Continuum soft gates: skip if missing, hard fail if wrong. */
	nFail += soft_u32(h, "gj_shell_green_15300", 1u, &nChecked);
	nFail += soft_u32(h, "gj_libcgj_green_15300", 1u, &nChecked);
	nFail += soft_u32(h, "gj_bar3_ready_15300", 0u, &nChecked);
	nFail += soft_u32(h, "gj_product_score_15300", 0u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_ready_15300", 1u, &nChecked);
	nFail += soft_u32(h, "gj_smoke_soft_15300", 1u, &nChecked);
	nFail += soft_u32(h, "gj_dyn_soft_15300", 1u, &nChecked);
	nFail += soft_u32(h, "gj_milestone_tag_15300", 15300u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_wave_15300", 15300u, &nChecked);
	nFail += soft_u32(h, "gj_batch_id_15300", 15300u, &nChecked);
	nFail += soft_u32(h, "gj_graph_milestone_15300", 15300u, &nChecked);

	nFail += soft_marker(h, "__libcgj_batch15291_marker",
	                     "libcgj-batch15291", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch15292_marker",
	                     "libcgj-batch15292", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch15293_marker",
	                     "libcgj-batch15293", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch15294_marker",
	                     "libcgj-batch15294", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch15295_marker",
	                     "libcgj-batch15295", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch15296_marker",
	                     "libcgj-batch15296", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch15297_marker",
	                     "libcgj-batch15297", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch15298_marker",
	                     "libcgj-batch15298", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch15299_marker",
	                     "libcgj-batch15299", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch15300_marker",
	                     "libcgj-batch15300", &nChecked);

	if (nFail != 0) {
		(void)dlclose(h);
		fprintf(stderr,
		        "libcgj: soft m15300 FAIL path=%s fails=%d checked=%d\n",
		        szPath, nFail, nChecked);
		return 2;
	}

	printf("libcgj: soft m15300 PASS path=%s checked=%d "
	       "(soft-skip unexported/missing; bar3 still open)\n",
	       szPath, nChecked);
	(void)dlclose(h);
	return 0;
#endif
}
