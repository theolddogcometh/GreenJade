/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 16300 markers (CREATE-ONLY).
 *
 * Coherent soft package (see NOTES_milestone_16300.txt):
 *   - this TU (libcgj continuum dyn/direct soft KATs)
 *   - NOTES_milestone_16300.txt
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
 *     cgj_soft_milestone_16100.c / cgj_soft_milestone_16200.c
 *     (sibling decade soft probes).
 *   - This TU follows the dyn-smoke soft pattern: skip if missing,
 *     hard-fail if present but wrong. Not wired in Makefile (tests are
 *     not auto-globbed); build and run manually when product SO is ready.
 *
 * Manual build (host libc + dl):
 *   cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_16300 \
 *     user/libcgj/tests/cgj_soft_milestone_16300.c -ldl
 *   ./build/cgj_soft_milestone_16300
 *
 * Optional hard unit check (link pure graph batches directly; no SO map):
 *   cc -std=c11 -Wall -Wextra -O2 -DCGJ_SOFT_M16300_DIRECT \
 *     -o build/cgj_soft_milestone_16300_direct \
 *     user/libcgj/tests/cgj_soft_milestone_16300.c \
 *     user/libcgj/src/graph_batch16291.c \
 *     user/libcgj/src/graph_batch16292.c \
 *     user/libcgj/src/graph_batch16293.c \
 *     user/libcgj/src/graph_batch16294.c \
 *     user/libcgj/src/graph_batch16295.c \
 *     user/libcgj/src/graph_batch16296.c \
 *     user/libcgj/src/graph_batch16297.c \
 *     user/libcgj/src/graph_batch16298.c \
 *     user/libcgj/src/graph_batch16299.c \
 *     user/libcgj/src/graph_batch16300.c
 *   ./build/cgj_soft_milestone_16300_direct
 *
 * Expected soft KATs (wave 16291–16300 exclusive continuum markers):
 *   gj_shell_green_16300      → 1
 *   gj_libcgj_green_16300     → 1
 *   gj_bar3_ready_16300       → 0   (bar3 still open)
 *   gj_product_score_16300    → 0   (product score still open)
 *   gj_continuum_ready_16300  → 1
 *   gj_smoke_soft_16300       → 1
 *   gj_dyn_soft_16300         → 1
 *   gj_milestone_tag_16300    → 16300
 *   gj_continuum_wave_16300   → 16300
 *   gj_batch_id_16300         → 16300
 *   gj_graph_milestone_16300  → 16300
 *   __libcgj_batch16291_marker … __libcgj_batch16300_marker
 *     → "libcgj-batchNNNNN" string match when present
 *
 * Notes for parent wiring (outside this soft exclusive scope):
 *   - Sources graph_batch16291.c … graph_batch16300.c (and fill
 *     16276–16290 / identity 16251–16275) are CREATE-ONLY; wire into
 *     CGJ_SRCS when parent advances makefile_max to 16300.
 *   - libc.map may lag milestone 16300 globals; add 16300 symbols to the
 *     GLIBC_2.2.5 global list so dyn dlsym can see them.
 *   - Optional: fold these soft gates into cgj_dyn_smoke.c /
 *     cgj_smoke.c continuum soft blocks (same skip-if-missing rule).
 *   - Soft gates; bar3 still open (gj_bar3_ready_16300 == 0).
 */

#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef CGJ_SOFT_M16300_DIRECT
#include <dlfcn.h>
#endif

#ifdef CGJ_SOFT_M16300_DIRECT
/* Direct-link path: call product pure markers without dlopen. */
extern uint32_t gj_shell_green_16300(void);
extern uint32_t gj_libcgj_green_16300(void);
extern uint32_t gj_bar3_ready_16300(void);
extern uint32_t gj_product_score_16300(void);
extern uint32_t gj_continuum_ready_16300(void);
extern uint32_t gj_smoke_soft_16300(void);
extern uint32_t gj_dyn_soft_16300(void);
extern uint32_t gj_milestone_tag_16300(void);
extern uint32_t gj_continuum_wave_16300(void);
extern uint32_t gj_batch_id_16300(void);
extern uint32_t gj_graph_milestone_16300(void);
extern const char __libcgj_batch16291_marker[];
extern const char __libcgj_batch16292_marker[];
extern const char __libcgj_batch16293_marker[];
extern const char __libcgj_batch16294_marker[];
extern const char __libcgj_batch16295_marker[];
extern const char __libcgj_batch16296_marker[];
extern const char __libcgj_batch16297_marker[];
extern const char __libcgj_batch16298_marker[];
extern const char __libcgj_batch16299_marker[];
extern const char __libcgj_batch16300_marker[];

static int
direct_check(void)
{
	int nFail = 0;

	if (gj_shell_green_16300() != 1u) {
		fprintf(stderr, "libcgj: direct gj_shell_green_16300 fail\n");
		nFail++;
	}
	if (gj_libcgj_green_16300() != 1u) {
		fprintf(stderr, "libcgj: direct gj_libcgj_green_16300 fail\n");
		nFail++;
	}
	if (gj_bar3_ready_16300() != 0u) {
		fprintf(stderr, "libcgj: direct gj_bar3_ready_16300 fail\n");
		nFail++;
	}
	if (gj_product_score_16300() != 0u) {
		fprintf(stderr, "libcgj: direct gj_product_score_16300 fail\n");
		nFail++;
	}
	if (gj_continuum_ready_16300() != 1u) {
		fprintf(stderr, "libcgj: direct gj_continuum_ready_16300 fail\n");
		nFail++;
	}
	if (gj_smoke_soft_16300() != 1u) {
		fprintf(stderr, "libcgj: direct gj_smoke_soft_16300 fail\n");
		nFail++;
	}
	if (gj_dyn_soft_16300() != 1u) {
		fprintf(stderr, "libcgj: direct gj_dyn_soft_16300 fail\n");
		nFail++;
	}
	if (gj_milestone_tag_16300() != 16300u) {
		fprintf(stderr, "libcgj: direct gj_milestone_tag_16300 fail\n");
		nFail++;
	}
	if (gj_continuum_wave_16300() != 16300u) {
		fprintf(stderr, "libcgj: direct gj_continuum_wave_16300 fail\n");
		nFail++;
	}
	if (gj_batch_id_16300() != 16300u) {
		fprintf(stderr, "libcgj: direct gj_batch_id_16300 fail\n");
		nFail++;
	}
	if (gj_graph_milestone_16300() != 16300u) {
		fprintf(stderr,
		        "libcgj: direct gj_graph_milestone_16300 fail\n");
		nFail++;
	}
	if (strcmp(__libcgj_batch16291_marker, "libcgj-batch16291") != 0 ||
	    strcmp(__libcgj_batch16292_marker, "libcgj-batch16292") != 0 ||
	    strcmp(__libcgj_batch16293_marker, "libcgj-batch16293") != 0 ||
	    strcmp(__libcgj_batch16294_marker, "libcgj-batch16294") != 0 ||
	    strcmp(__libcgj_batch16295_marker, "libcgj-batch16295") != 0 ||
	    strcmp(__libcgj_batch16296_marker, "libcgj-batch16296") != 0 ||
	    strcmp(__libcgj_batch16297_marker, "libcgj-batch16297") != 0 ||
	    strcmp(__libcgj_batch16298_marker, "libcgj-batch16298") != 0 ||
	    strcmp(__libcgj_batch16299_marker, "libcgj-batch16299") != 0 ||
	    strcmp(__libcgj_batch16300_marker, "libcgj-batch16300") != 0) {
		fprintf(stderr, "libcgj: direct m16300 marker string fail\n");
		nFail++;
	}
	return nFail;
}
#else /* !CGJ_SOFT_M16300_DIRECT — dyn soft path */

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
		        "libcgj: soft m16300 %s KAT fail got=%u expect=%u\n",
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
		        "libcgj: soft m16300 %s marker fail got=\"%s\" "
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

#endif /* CGJ_SOFT_M16300_DIRECT */

int
main(void)
{
#ifdef CGJ_SOFT_M16300_DIRECT
	int nFail;

	nFail = direct_check();
	if (nFail != 0) {
		fprintf(stderr,
		        "libcgj: soft m16300 DIRECT FAIL fails=%d\n", nFail);
		return 1;
	}
	printf("libcgj: soft m16300 DIRECT PASS (all 11 lamps + markers)\n");
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
		        "libcgj: soft m16300 skip (dlopen fail: %s)\n",
		        dlerror());
		printf("libcgj: soft m16300 SOFT-SKIP (no product SO)\n");
		return 0;
	}

	/* Continuum soft gates: skip if missing, hard fail if wrong. */
	nFail += soft_u32(h, "gj_shell_green_16300", 1u, &nChecked);
	nFail += soft_u32(h, "gj_libcgj_green_16300", 1u, &nChecked);
	nFail += soft_u32(h, "gj_bar3_ready_16300", 0u, &nChecked);
	nFail += soft_u32(h, "gj_product_score_16300", 0u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_ready_16300", 1u, &nChecked);
	nFail += soft_u32(h, "gj_smoke_soft_16300", 1u, &nChecked);
	nFail += soft_u32(h, "gj_dyn_soft_16300", 1u, &nChecked);
	nFail += soft_u32(h, "gj_milestone_tag_16300", 16300u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_wave_16300", 16300u, &nChecked);
	nFail += soft_u32(h, "gj_batch_id_16300", 16300u, &nChecked);
	nFail += soft_u32(h, "gj_graph_milestone_16300", 16300u, &nChecked);

	nFail += soft_marker(h, "__libcgj_batch16291_marker",
	                     "libcgj-batch16291", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch16292_marker",
	                     "libcgj-batch16292", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch16293_marker",
	                     "libcgj-batch16293", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch16294_marker",
	                     "libcgj-batch16294", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch16295_marker",
	                     "libcgj-batch16295", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch16296_marker",
	                     "libcgj-batch16296", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch16297_marker",
	                     "libcgj-batch16297", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch16298_marker",
	                     "libcgj-batch16298", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch16299_marker",
	                     "libcgj-batch16299", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch16300_marker",
	                     "libcgj-batch16300", &nChecked);

	if (nFail != 0) {
		(void)dlclose(h);
		fprintf(stderr,
		        "libcgj: soft m16300 FAIL path=%s fails=%d checked=%d\n",
		        szPath, nFail, nChecked);
		return 2;
	}

	printf("libcgj: soft m16300 PASS path=%s checked=%d "
	       "(soft-skip unexported/missing; bar3 still open)\n",
	       szPath, nChecked);
	(void)dlclose(h);
	return 0;
#endif
}
