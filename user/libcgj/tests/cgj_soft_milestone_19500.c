/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 19500 markers (CREATE-ONLY).
 *
 * Coherent soft package (see NOTES_milestone_19500.txt):
 *   - this TU (libcgj continuum dyn/direct soft KATs)
 *   - NOTES_milestone_19500.txt
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
 *     (sibling decade soft probes).
 *   - This TU follows the dyn-smoke soft pattern: skip if missing,
 *     hard-fail if present but wrong. Not wired in Makefile (tests are
 *     not auto-globbed); build and run manually when product SO is ready.
 *
 * Manual build (host libc + dl):
 *   cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_19500 \
 *     user/libcgj/tests/cgj_soft_milestone_19500.c -ldl
 *   ./build/cgj_soft_milestone_19500
 *
 * Optional hard unit check (link pure graph batches directly; no SO map):
 *   cc -std=c11 -Wall -Wextra -O2 -DCGJ_SOFT_M19500_DIRECT \
 *     -o build/cgj_soft_milestone_19500_direct \
 *     user/libcgj/tests/cgj_soft_milestone_19500.c \
 *     user/libcgj/src/graph_batch19491.c \
 *     user/libcgj/src/graph_batch19492.c \
 *     user/libcgj/src/graph_batch19493.c \
 *     user/libcgj/src/graph_batch19494.c \
 *     user/libcgj/src/graph_batch19495.c \
 *     user/libcgj/src/graph_batch19496.c \
 *     user/libcgj/src/graph_batch19497.c \
 *     user/libcgj/src/graph_batch19498.c \
 *     user/libcgj/src/graph_batch19499.c \
 *     user/libcgj/src/graph_batch19500.c
 *   ./build/cgj_soft_milestone_19500_direct
 *
 * Expected soft KATs (wave 19491–19500 exclusive continuum markers):
 *   gj_shell_green_19500      → 1
 *   gj_libcgj_green_19500     → 1
 *   gj_bar3_ready_19500       → 0   (bar3 still open)
 *   gj_product_score_19500    → 0   (product score still open)
 *   gj_continuum_ready_19500  → 1
 *   gj_smoke_soft_19500       → 1
 *   gj_dyn_soft_19500         → 1
 *   gj_milestone_tag_19500    → 19500
 *   gj_continuum_wave_19500   → 19500
 *   gj_batch_id_19500         → 19500
 *   gj_graph_milestone_19500  → 19500
 *   __libcgj_batch19491_marker … __libcgj_batch19500_marker
 *     → "libcgj-batchNNNNN" string match when present
 *
 * Notes for parent wiring (outside this soft exclusive scope):
 *   - Sources graph_batch19491.c … graph_batch19500.c (and fill
 *     19476–19490 / identity 19451–19475) are CREATE-ONLY; wire into
 *     CGJ_SRCS when parent advances makefile_max to 19500.
 *   - libc.map may lag milestone 19500 globals; add 19500 symbols to the
 *     GLIBC_2.2.5 global list so dyn dlsym can see them.
 *   - Optional: fold these soft gates into cgj_dyn_smoke.c /
 *     cgj_smoke.c continuum soft blocks (same skip-if-missing rule).
 *   - Soft gates; bar3 still open (gj_bar3_ready_19500 == 0).
 */

#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef CGJ_SOFT_M19500_DIRECT
#include <dlfcn.h>
#endif

#ifdef CGJ_SOFT_M19500_DIRECT
/* Direct-link path: call product pure markers without dlopen. */
extern uint32_t gj_shell_green_19500(void);
extern uint32_t gj_libcgj_green_19500(void);
extern uint32_t gj_bar3_ready_19500(void);
extern uint32_t gj_product_score_19500(void);
extern uint32_t gj_continuum_ready_19500(void);
extern uint32_t gj_smoke_soft_19500(void);
extern uint32_t gj_dyn_soft_19500(void);
extern uint32_t gj_milestone_tag_19500(void);
extern uint32_t gj_continuum_wave_19500(void);
extern uint32_t gj_batch_id_19500(void);
extern uint32_t gj_graph_milestone_19500(void);
extern const char __libcgj_batch19491_marker[];
extern const char __libcgj_batch19492_marker[];
extern const char __libcgj_batch19493_marker[];
extern const char __libcgj_batch19494_marker[];
extern const char __libcgj_batch19495_marker[];
extern const char __libcgj_batch19496_marker[];
extern const char __libcgj_batch19497_marker[];
extern const char __libcgj_batch19498_marker[];
extern const char __libcgj_batch19499_marker[];
extern const char __libcgj_batch19500_marker[];

static int
direct_check(void)
{
	int nFail = 0;

	if (gj_shell_green_19500() != 1u) {
		fprintf(stderr, "libcgj: direct gj_shell_green_19500 fail\n");
		nFail++;
	}
	if (gj_libcgj_green_19500() != 1u) {
		fprintf(stderr, "libcgj: direct gj_libcgj_green_19500 fail\n");
		nFail++;
	}
	if (gj_bar3_ready_19500() != 0u) {
		fprintf(stderr, "libcgj: direct gj_bar3_ready_19500 fail\n");
		nFail++;
	}
	if (gj_product_score_19500() != 0u) {
		fprintf(stderr, "libcgj: direct gj_product_score_19500 fail\n");
		nFail++;
	}
	if (gj_continuum_ready_19500() != 1u) {
		fprintf(stderr, "libcgj: direct gj_continuum_ready_19500 fail\n");
		nFail++;
	}
	if (gj_smoke_soft_19500() != 1u) {
		fprintf(stderr, "libcgj: direct gj_smoke_soft_19500 fail\n");
		nFail++;
	}
	if (gj_dyn_soft_19500() != 1u) {
		fprintf(stderr, "libcgj: direct gj_dyn_soft_19500 fail\n");
		nFail++;
	}
	if (gj_milestone_tag_19500() != 19500u) {
		fprintf(stderr, "libcgj: direct gj_milestone_tag_19500 fail\n");
		nFail++;
	}
	if (gj_continuum_wave_19500() != 19500u) {
		fprintf(stderr, "libcgj: direct gj_continuum_wave_19500 fail\n");
		nFail++;
	}
	if (gj_batch_id_19500() != 19500u) {
		fprintf(stderr, "libcgj: direct gj_batch_id_19500 fail\n");
		nFail++;
	}
	if (gj_graph_milestone_19500() != 19500u) {
		fprintf(stderr,
		        "libcgj: direct gj_graph_milestone_19500 fail\n");
		nFail++;
	}
	if (strcmp(__libcgj_batch19491_marker, "libcgj-batch19491") != 0 ||
	    strcmp(__libcgj_batch19492_marker, "libcgj-batch19492") != 0 ||
	    strcmp(__libcgj_batch19493_marker, "libcgj-batch19493") != 0 ||
	    strcmp(__libcgj_batch19494_marker, "libcgj-batch19494") != 0 ||
	    strcmp(__libcgj_batch19495_marker, "libcgj-batch19495") != 0 ||
	    strcmp(__libcgj_batch19496_marker, "libcgj-batch19496") != 0 ||
	    strcmp(__libcgj_batch19497_marker, "libcgj-batch19497") != 0 ||
	    strcmp(__libcgj_batch19498_marker, "libcgj-batch19498") != 0 ||
	    strcmp(__libcgj_batch19499_marker, "libcgj-batch19499") != 0 ||
	    strcmp(__libcgj_batch19500_marker, "libcgj-batch19500") != 0) {
		fprintf(stderr, "libcgj: direct m19500 marker string fail\n");
		nFail++;
	}
	return nFail;
}
#else /* !CGJ_SOFT_M19500_DIRECT — dyn soft path */

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
		        "libcgj: soft m19500 %s KAT fail got=%u expect=%u\n",
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
		        "libcgj: soft m19500 %s marker fail got=\"%s\" "
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

#endif /* CGJ_SOFT_M19500_DIRECT */

int
main(void)
{
#ifdef CGJ_SOFT_M19500_DIRECT
	int nFail;

	nFail = direct_check();
	if (nFail != 0) {
		fprintf(stderr,
		        "libcgj: soft m19500 DIRECT FAIL fails=%d\n", nFail);
		return 1;
	}
	printf("libcgj: soft m19500 DIRECT PASS (all 11 lamps + markers)\n");
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
		        "libcgj: soft m19500 skip (dlopen fail: %s)\n",
		        dlerror());
		printf("libcgj: soft m19500 SOFT-SKIP (no product SO)\n");
		return 0;
	}

	/* Continuum soft gates: skip if missing, hard fail if wrong. */
	nFail += soft_u32(h, "gj_shell_green_19500", 1u, &nChecked);
	nFail += soft_u32(h, "gj_libcgj_green_19500", 1u, &nChecked);
	nFail += soft_u32(h, "gj_bar3_ready_19500", 0u, &nChecked);
	nFail += soft_u32(h, "gj_product_score_19500", 0u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_ready_19500", 1u, &nChecked);
	nFail += soft_u32(h, "gj_smoke_soft_19500", 1u, &nChecked);
	nFail += soft_u32(h, "gj_dyn_soft_19500", 1u, &nChecked);
	nFail += soft_u32(h, "gj_milestone_tag_19500", 19500u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_wave_19500", 19500u, &nChecked);
	nFail += soft_u32(h, "gj_batch_id_19500", 19500u, &nChecked);
	nFail += soft_u32(h, "gj_graph_milestone_19500", 19500u, &nChecked);

	nFail += soft_marker(h, "__libcgj_batch19491_marker",
	                     "libcgj-batch19491", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch19492_marker",
	                     "libcgj-batch19492", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch19493_marker",
	                     "libcgj-batch19493", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch19494_marker",
	                     "libcgj-batch19494", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch19495_marker",
	                     "libcgj-batch19495", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch19496_marker",
	                     "libcgj-batch19496", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch19497_marker",
	                     "libcgj-batch19497", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch19498_marker",
	                     "libcgj-batch19498", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch19499_marker",
	                     "libcgj-batch19499", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch19500_marker",
	                     "libcgj-batch19500", &nChecked);

	if (nFail != 0) {
		(void)dlclose(h);
		fprintf(stderr,
		        "libcgj: soft m19500 FAIL path=%s fails=%d checked=%d\n",
		        szPath, nFail, nChecked);
		return 2;
	}

	printf("libcgj: soft m19500 PASS path=%s checked=%d "
	       "(soft-skip unexported/missing; bar3 still open)\n",
	       szPath, nChecked);
	(void)dlclose(h);
	return 0;
#endif
}
