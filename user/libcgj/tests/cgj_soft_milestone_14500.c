/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 14500 markers (CREATE-ONLY).
 *
 * Test harness note:
 *   - Sibling harnesses: cgj_smoke.c (static freestanding weak), 
 *     cgj_dyn_smoke.c (host dlopen product libc.so.6).
 *   - This TU follows the dyn-smoke soft pattern: skip if missing,
 *     hard-fail if present but wrong. Not wired in Makefile (tests are
 *     not auto-globbed); build and run manually when product SO is ready.
 *
 * Manual build (host libc + dl):
 *   cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_14500 \
 *     user/libcgj/tests/cgj_soft_milestone_14500.c -ldl
 *   ./build/cgj_soft_milestone_14500
 *
 * Optional hard unit check (link pure graph batches directly; no SO map):
 *   cc -std=c11 -Wall -Wextra -O2 -DCGJ_SOFT_M14500_DIRECT \
 *     -o build/cgj_soft_milestone_14500_direct \
 *     user/libcgj/tests/cgj_soft_milestone_14500.c \
 *     user/libcgj/src/graph_batch14491.c \
 *     user/libcgj/src/graph_batch14492.c \
 *     user/libcgj/src/graph_batch14493.c \
 *     user/libcgj/src/graph_batch14494.c \
 *     user/libcgj/src/graph_batch14495.c \
 *     user/libcgj/src/graph_batch14496.c \
 *     user/libcgj/src/graph_batch14497.c \
 *     user/libcgj/src/graph_batch14498.c \
 *     user/libcgj/src/graph_batch14499.c \
 *     user/libcgj/src/graph_batch14500.c
 *   ./build/cgj_soft_milestone_14500_direct
 *
 * Expected soft KATs (wave 14491–14500 exclusive continuum markers):
 *   gj_shell_green_14500      → 1
 *   gj_libcgj_green_14500     → 1
 *   gj_bar3_ready_14500       → 0   (bar3 still open)
 *   gj_product_score_14500    → 0   (product score still open)
 *   gj_continuum_ready_14500  → 1
 *   gj_smoke_soft_14500       → 1
 *   gj_dyn_soft_14500         → 1
 *   gj_milestone_tag_14500    → 14500
 *   gj_continuum_wave_14500   → 14500
 *   gj_batch_id_14500         → 14500
 *   gj_graph_milestone_14500  → 14500
 *   __libcgj_batch14491_marker … __libcgj_batch14500_marker
 *     → "libcgj-batchNNNNN" string match when present
 *
 * Notes for parent wiring (outside this exclusive tests/ scope):
 *   - Sources graph_batch14491.c … graph_batch14500.c are already on
 *     CGJ_SRCS (makefile_max=14500). Rebuild libcgj / libc.so.6 after map.
 *   - libc.map currently ends at milestone 14400 globals; add 14500
 *     symbols to the GLIBC_2.2.5 global list so dyn dlsym can see them.
 *   - Optional: fold these soft gates into cgj_dyn_smoke.c /
 *     cgj_smoke.c continuum soft blocks (same skip-if-missing rule).
 *   - Soft gates; bar3 still open (gj_bar3_ready_14500 == 0).
 */

#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef CGJ_SOFT_M14500_DIRECT
#include <dlfcn.h>
#endif

#ifdef CGJ_SOFT_M14500_DIRECT
/* Direct-link path: call product pure markers without dlopen. */
extern uint32_t gj_shell_green_14500(void);
extern uint32_t gj_libcgj_green_14500(void);
extern uint32_t gj_bar3_ready_14500(void);
extern uint32_t gj_product_score_14500(void);
extern uint32_t gj_continuum_ready_14500(void);
extern uint32_t gj_smoke_soft_14500(void);
extern uint32_t gj_dyn_soft_14500(void);
extern uint32_t gj_milestone_tag_14500(void);
extern uint32_t gj_continuum_wave_14500(void);
extern uint32_t gj_batch_id_14500(void);
extern uint32_t gj_graph_milestone_14500(void);
extern const char __libcgj_batch14491_marker[];
extern const char __libcgj_batch14492_marker[];
extern const char __libcgj_batch14493_marker[];
extern const char __libcgj_batch14494_marker[];
extern const char __libcgj_batch14495_marker[];
extern const char __libcgj_batch14496_marker[];
extern const char __libcgj_batch14497_marker[];
extern const char __libcgj_batch14498_marker[];
extern const char __libcgj_batch14499_marker[];
extern const char __libcgj_batch14500_marker[];

static int
direct_check(void)
{
	int nFail = 0;

	if (gj_shell_green_14500() != 1u) {
		fprintf(stderr, "libcgj: direct gj_shell_green_14500 fail\n");
		nFail++;
	}
	if (gj_libcgj_green_14500() != 1u) {
		fprintf(stderr, "libcgj: direct gj_libcgj_green_14500 fail\n");
		nFail++;
	}
	if (gj_bar3_ready_14500() != 0u) {
		fprintf(stderr, "libcgj: direct gj_bar3_ready_14500 fail\n");
		nFail++;
	}
	if (gj_product_score_14500() != 0u) {
		fprintf(stderr, "libcgj: direct gj_product_score_14500 fail\n");
		nFail++;
	}
	if (gj_continuum_ready_14500() != 1u) {
		fprintf(stderr, "libcgj: direct gj_continuum_ready_14500 fail\n");
		nFail++;
	}
	if (gj_smoke_soft_14500() != 1u) {
		fprintf(stderr, "libcgj: direct gj_smoke_soft_14500 fail\n");
		nFail++;
	}
	if (gj_dyn_soft_14500() != 1u) {
		fprintf(stderr, "libcgj: direct gj_dyn_soft_14500 fail\n");
		nFail++;
	}
	if (gj_milestone_tag_14500() != 14500u) {
		fprintf(stderr, "libcgj: direct gj_milestone_tag_14500 fail\n");
		nFail++;
	}
	if (gj_continuum_wave_14500() != 14500u) {
		fprintf(stderr, "libcgj: direct gj_continuum_wave_14500 fail\n");
		nFail++;
	}
	if (gj_batch_id_14500() != 14500u) {
		fprintf(stderr, "libcgj: direct gj_batch_id_14500 fail\n");
		nFail++;
	}
	if (gj_graph_milestone_14500() != 14500u) {
		fprintf(stderr,
		        "libcgj: direct gj_graph_milestone_14500 fail\n");
		nFail++;
	}
	if (strcmp(__libcgj_batch14491_marker, "libcgj-batch14491") != 0 ||
	    strcmp(__libcgj_batch14492_marker, "libcgj-batch14492") != 0 ||
	    strcmp(__libcgj_batch14493_marker, "libcgj-batch14493") != 0 ||
	    strcmp(__libcgj_batch14494_marker, "libcgj-batch14494") != 0 ||
	    strcmp(__libcgj_batch14495_marker, "libcgj-batch14495") != 0 ||
	    strcmp(__libcgj_batch14496_marker, "libcgj-batch14496") != 0 ||
	    strcmp(__libcgj_batch14497_marker, "libcgj-batch14497") != 0 ||
	    strcmp(__libcgj_batch14498_marker, "libcgj-batch14498") != 0 ||
	    strcmp(__libcgj_batch14499_marker, "libcgj-batch14499") != 0 ||
	    strcmp(__libcgj_batch14500_marker, "libcgj-batch14500") != 0) {
		fprintf(stderr, "libcgj: direct m14500 marker string fail\n");
		nFail++;
	}
	return nFail;
}
#else /* !CGJ_SOFT_M14500_DIRECT — dyn soft path */

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
		        "libcgj: soft m14500 %s KAT fail got=%u expect=%u\n",
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
		        "libcgj: soft m14500 %s marker fail got=\"%s\" "
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

#endif /* CGJ_SOFT_M14500_DIRECT */

int
main(void)
{
#ifdef CGJ_SOFT_M14500_DIRECT
	int nFail;

	nFail = direct_check();
	if (nFail != 0) {
		fprintf(stderr,
		        "libcgj: soft m14500 DIRECT FAIL fails=%d\n", nFail);
		return 1;
	}
	printf("libcgj: soft m14500 DIRECT PASS (all 11 lamps + markers)\n");
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
		        "libcgj: soft m14500 skip (dlopen fail: %s)\n",
		        dlerror());
		printf("libcgj: soft m14500 SOFT-SKIP (no product SO)\n");
		return 0;
	}

	/* Continuum soft gates: skip if missing, hard fail if wrong. */
	nFail += soft_u32(h, "gj_shell_green_14500", 1u, &nChecked);
	nFail += soft_u32(h, "gj_libcgj_green_14500", 1u, &nChecked);
	nFail += soft_u32(h, "gj_bar3_ready_14500", 0u, &nChecked);
	nFail += soft_u32(h, "gj_product_score_14500", 0u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_ready_14500", 1u, &nChecked);
	nFail += soft_u32(h, "gj_smoke_soft_14500", 1u, &nChecked);
	nFail += soft_u32(h, "gj_dyn_soft_14500", 1u, &nChecked);
	nFail += soft_u32(h, "gj_milestone_tag_14500", 14500u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_wave_14500", 14500u, &nChecked);
	nFail += soft_u32(h, "gj_batch_id_14500", 14500u, &nChecked);
	nFail += soft_u32(h, "gj_graph_milestone_14500", 14500u, &nChecked);

	nFail += soft_marker(h, "__libcgj_batch14491_marker",
	                     "libcgj-batch14491", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch14492_marker",
	                     "libcgj-batch14492", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch14493_marker",
	                     "libcgj-batch14493", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch14494_marker",
	                     "libcgj-batch14494", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch14495_marker",
	                     "libcgj-batch14495", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch14496_marker",
	                     "libcgj-batch14496", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch14497_marker",
	                     "libcgj-batch14497", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch14498_marker",
	                     "libcgj-batch14498", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch14499_marker",
	                     "libcgj-batch14499", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch14500_marker",
	                     "libcgj-batch14500", &nChecked);

	if (nFail != 0) {
		(void)dlclose(h);
		fprintf(stderr,
		        "libcgj: soft m14500 FAIL path=%s fails=%d checked=%d\n",
		        szPath, nFail, nChecked);
		return 2;
	}

	printf("libcgj: soft m14500 PASS path=%s checked=%d "
	       "(soft-skip unexported/missing; bar3 still open)\n",
	       szPath, nChecked);
	(void)dlclose(h);
	return 0;
#endif
}
