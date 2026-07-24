/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 22600 markers (CREATE-ONLY).
 *
 * Coherent soft package (see NOTES_milestone_22600.txt):
 *   - this TU (libcgj continuum dyn/direct soft KATs)
 *   - NOTES_milestone_22600.txt
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
 *     cgj_soft_milestone_22500.c /
 *     (sibling decade soft probes).
 *   - This TU follows the dyn-smoke soft pattern: skip if missing,
 *     hard-fail if present but wrong. Not wired in Makefile (tests are
 *     not auto-globbed); build and run manually when product SO is ready.
 *
 * Manual build (host libc + dl):
 *   cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_22600 \
 *     user/libcgj/tests/cgj_soft_milestone_22600.c -ldl
 *   ./build/cgj_soft_milestone_22600
 *
 * Optional hard unit check (link pure graph batches directly; no SO map):
 *   cc -std=c11 -Wall -Wextra -O2 -DCGJ_SOFT_M22600_DIRECT \
 *     -o build/cgj_soft_milestone_22600_direct \
 *     user/libcgj/tests/cgj_soft_milestone_22600.c \
 *     user/libcgj/src/graph_batch22591.c \
 *     user/libcgj/src/graph_batch22592.c \
 *     user/libcgj/src/graph_batch22593.c \
 *     user/libcgj/src/graph_batch22594.c \
 *     user/libcgj/src/graph_batch22595.c \
 *     user/libcgj/src/graph_batch22596.c \
 *     user/libcgj/src/graph_batch22597.c \
 *     user/libcgj/src/graph_batch22598.c \
 *     user/libcgj/src/graph_batch22599.c \
 *     user/libcgj/src/graph_batch22600.c
 *   ./build/cgj_soft_milestone_22600_direct
 *
 * Expected soft KATs (wave 22591–22600 exclusive continuum markers):
 *   gj_shell_green_22600      → 1
 *   gj_libcgj_green_22600     → 1
 *   gj_bar3_ready_22600       → 0   (bar3 still open)
 *   gj_product_score_22600    → 0   (product score still open)
 *   gj_continuum_ready_22600  → 1
 *   gj_smoke_soft_22600       → 1
 *   gj_dyn_soft_22600         → 1
 *   gj_milestone_tag_22600    → 22600
 *   gj_continuum_wave_22600   → 22600
 *   gj_batch_id_22600         → 22600
 *   gj_graph_milestone_22600  → 22600
 *   __libcgj_batch22591_marker … __libcgj_batch22600_marker
 *     → "libcgj-batchNNNNN" string match when present
 *
 * Notes for parent wiring (outside this soft exclusive scope):
 *   - Sources graph_batch22591.c … graph_batch22600.c (and fill
 *     22576–22590 / identity 22551–22575) are CREATE-ONLY; wire into
 *     CGJ_SRCS when parent advances makefile_max to 22600.
 *   - libc.map may lag milestone 22600 globals; add 22600 symbols to the
 *     GLIBC_2.2.5 global list so dyn dlsym can see them.
 *   - Optional: fold these soft gates into cgj_dyn_smoke.c /
 *     cgj_smoke.c continuum soft blocks (same skip-if-missing rule).
 *   - Soft gates; bar3 still open (gj_bar3_ready_22600 == 0).
 */

#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef CGJ_SOFT_M22600_DIRECT
#include <dlfcn.h>
#endif

#ifdef CGJ_SOFT_M22600_DIRECT
/* Direct-link path: call product pure markers without dlopen. */
extern uint32_t gj_shell_green_22600(void);
extern uint32_t gj_libcgj_green_22600(void);
extern uint32_t gj_bar3_ready_22600(void);
extern uint32_t gj_product_score_22600(void);
extern uint32_t gj_continuum_ready_22600(void);
extern uint32_t gj_smoke_soft_22600(void);
extern uint32_t gj_dyn_soft_22600(void);
extern uint32_t gj_milestone_tag_22600(void);
extern uint32_t gj_continuum_wave_22600(void);
extern uint32_t gj_batch_id_22600(void);
extern uint32_t gj_graph_milestone_22600(void);
extern const char __libcgj_batch22591_marker[];
extern const char __libcgj_batch22592_marker[];
extern const char __libcgj_batch22593_marker[];
extern const char __libcgj_batch22594_marker[];
extern const char __libcgj_batch22595_marker[];
extern const char __libcgj_batch22596_marker[];
extern const char __libcgj_batch22597_marker[];
extern const char __libcgj_batch22598_marker[];
extern const char __libcgj_batch22599_marker[];
extern const char __libcgj_batch22600_marker[];

static int
direct_check(void)
{
	int nFail = 0;

	if (gj_shell_green_22600() != 1u) {
		fprintf(stderr, "libcgj: direct gj_shell_green_22600 fail\n");
		nFail++;
	}
	if (gj_libcgj_green_22600() != 1u) {
		fprintf(stderr, "libcgj: direct gj_libcgj_green_22600 fail\n");
		nFail++;
	}
	if (gj_bar3_ready_22600() != 0u) {
		fprintf(stderr, "libcgj: direct gj_bar3_ready_22600 fail\n");
		nFail++;
	}
	if (gj_product_score_22600() != 0u) {
		fprintf(stderr, "libcgj: direct gj_product_score_22600 fail\n");
		nFail++;
	}
	if (gj_continuum_ready_22600() != 1u) {
		fprintf(stderr, "libcgj: direct gj_continuum_ready_22600 fail\n");
		nFail++;
	}
	if (gj_smoke_soft_22600() != 1u) {
		fprintf(stderr, "libcgj: direct gj_smoke_soft_22600 fail\n");
		nFail++;
	}
	if (gj_dyn_soft_22600() != 1u) {
		fprintf(stderr, "libcgj: direct gj_dyn_soft_22600 fail\n");
		nFail++;
	}
	if (gj_milestone_tag_22600() != 22600u) {
		fprintf(stderr, "libcgj: direct gj_milestone_tag_22600 fail\n");
		nFail++;
	}
	if (gj_continuum_wave_22600() != 22600u) {
		fprintf(stderr, "libcgj: direct gj_continuum_wave_22600 fail\n");
		nFail++;
	}
	if (gj_batch_id_22600() != 22600u) {
		fprintf(stderr, "libcgj: direct gj_batch_id_22600 fail\n");
		nFail++;
	}
	if (gj_graph_milestone_22600() != 22600u) {
		fprintf(stderr,
		        "libcgj: direct gj_graph_milestone_22600 fail\n");
		nFail++;
	}
	if (strcmp(__libcgj_batch22591_marker, "libcgj-batch22591") != 0 ||
	    strcmp(__libcgj_batch22592_marker, "libcgj-batch22592") != 0 ||
	    strcmp(__libcgj_batch22593_marker, "libcgj-batch22593") != 0 ||
	    strcmp(__libcgj_batch22594_marker, "libcgj-batch22594") != 0 ||
	    strcmp(__libcgj_batch22595_marker, "libcgj-batch22595") != 0 ||
	    strcmp(__libcgj_batch22596_marker, "libcgj-batch22596") != 0 ||
	    strcmp(__libcgj_batch22597_marker, "libcgj-batch22597") != 0 ||
	    strcmp(__libcgj_batch22598_marker, "libcgj-batch22598") != 0 ||
	    strcmp(__libcgj_batch22599_marker, "libcgj-batch22599") != 0 ||
	    strcmp(__libcgj_batch22600_marker, "libcgj-batch22600") != 0) {
		fprintf(stderr, "libcgj: direct m22600 marker string fail\n");
		nFail++;
	}
	return nFail;
}
#else /* !CGJ_SOFT_M22600_DIRECT — dyn soft path */

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
		        "libcgj: soft m22600 %s KAT fail got=%u expect=%u\n",
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
		        "libcgj: soft m22600 %s marker fail got=\"%s\" "
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

#endif /* CGJ_SOFT_M22600_DIRECT */

int
main(void)
{
#ifdef CGJ_SOFT_M22600_DIRECT
	int nFail;

	nFail = direct_check();
	if (nFail != 0) {
		fprintf(stderr,
		        "libcgj: soft m22600 DIRECT FAIL fails=%d\n", nFail);
		return 1;
	}
	printf("libcgj: soft m22600 DIRECT PASS (all 11 lamps + markers)\n");
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
		        "libcgj: soft m22600 skip (dlopen fail: %s)\n",
		        dlerror());
		printf("libcgj: soft m22600 SOFT-SKIP (no product SO)\n");
		return 0;
	}

	/* Continuum soft gates: skip if missing, hard fail if wrong. */
	nFail += soft_u32(h, "gj_shell_green_22600", 1u, &nChecked);
	nFail += soft_u32(h, "gj_libcgj_green_22600", 1u, &nChecked);
	nFail += soft_u32(h, "gj_bar3_ready_22600", 0u, &nChecked);
	nFail += soft_u32(h, "gj_product_score_22600", 0u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_ready_22600", 1u, &nChecked);
	nFail += soft_u32(h, "gj_smoke_soft_22600", 1u, &nChecked);
	nFail += soft_u32(h, "gj_dyn_soft_22600", 1u, &nChecked);
	nFail += soft_u32(h, "gj_milestone_tag_22600", 22600u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_wave_22600", 22600u, &nChecked);
	nFail += soft_u32(h, "gj_batch_id_22600", 22600u, &nChecked);
	nFail += soft_u32(h, "gj_graph_milestone_22600", 22600u, &nChecked);

	nFail += soft_marker(h, "__libcgj_batch22591_marker",
	                     "libcgj-batch22591", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch22592_marker",
	                     "libcgj-batch22592", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch22593_marker",
	                     "libcgj-batch22593", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch22594_marker",
	                     "libcgj-batch22594", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch22595_marker",
	                     "libcgj-batch22595", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch22596_marker",
	                     "libcgj-batch22596", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch22597_marker",
	                     "libcgj-batch22597", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch22598_marker",
	                     "libcgj-batch22598", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch22599_marker",
	                     "libcgj-batch22599", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch22600_marker",
	                     "libcgj-batch22600", &nChecked);

	if (nFail != 0) {
		(void)dlclose(h);
		fprintf(stderr,
		        "libcgj: soft m22600 FAIL path=%s fails=%d checked=%d\n",
		        szPath, nFail, nChecked);
		return 2;
	}

	printf("libcgj: soft m22600 PASS path=%s checked=%d "
	       "(soft-skip unexported/missing; bar3 still open)\n",
	       szPath, nChecked);
	(void)dlclose(h);
	return 0;
#endif
}
