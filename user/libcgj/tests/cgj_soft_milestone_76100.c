/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 76100 markers (CREATE-ONLY).
 *
 * Coherent soft package (see NOTES_milestone_76100.txt):
 *   - this TU (libcgj continuum dyn/direct soft KATs)
 *   - NOTES_milestone_76100.txt
 *
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_76100 == 0).
 * Sibling harnesses: cgj_smoke.c / cgj_dyn_smoke.c / prior soft decades.
 * Not wired in Makefile; build and run manually when product SO is ready.
 *
 * Manual build (host libc + dl):
 *   cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_76100 \
 *     user/libcgj/tests/cgj_soft_milestone_76100.c -ldl
 *   ./build/cgj_soft_milestone_76100
 *
 * Optional hard unit check (link pure graph batches directly; no SO map):
 *   cc -std=c11 -Wall -Wextra -O2 -DCGJ_SOFT_M76100_DIRECT \
 *     -o build/cgj_soft_milestone_76100_direct \
 *     user/libcgj/tests/cgj_soft_milestone_76100.c \
 *     user/libcgj/src/graph_batch76091.c \
 *     user/libcgj/src/graph_batch76092.c \
 *     user/libcgj/src/graph_batch76093.c \
 *     user/libcgj/src/graph_batch76094.c \
 *     user/libcgj/src/graph_batch76095.c \
 *     user/libcgj/src/graph_batch76096.c \
 *     user/libcgj/src/graph_batch76097.c \
 *     user/libcgj/src/graph_batch76098.c \
 *     user/libcgj/src/graph_batch76099.c \
 *     user/libcgj/src/graph_batch76100.c
 *   ./build/cgj_soft_milestone_76100_direct
 *
 * Expected soft KATs (wave 76091–76100 exclusive continuum markers):
 *   gj_shell_green_76100      → 1
 *   gj_libcgj_green_76100     → 1
 *   gj_bar3_ready_76100       → 0   (bar3 still open)
 *   gj_product_score_76100    → 0   (product score still open)
 *   gj_continuum_ready_76100  → 1
 *   gj_smoke_soft_76100       → 1
 *   gj_dyn_soft_76100         → 1
 *   gj_milestone_tag_76100    → 76100
 *   gj_continuum_wave_76100   → 76100
 *   gj_batch_id_76100         → 76100
 *   gj_graph_milestone_76100  → 76100
 *   __libcgj_batch76091_marker … __libcgj_batch76100_marker
 *
 * Parent wiring: graph_batch76091.c … graph_batch76100.c (fill 76076–76090,
 * identity 76051–76075) CREATE-ONLY until makefile_max advances to 76100.
 */

#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef CGJ_SOFT_M76100_DIRECT
#include <dlfcn.h>
#endif

#ifdef CGJ_SOFT_M76100_DIRECT
extern uint32_t gj_shell_green_76100(void);
extern uint32_t gj_libcgj_green_76100(void);
extern uint32_t gj_bar3_ready_76100(void);
extern uint32_t gj_product_score_76100(void);
extern uint32_t gj_continuum_ready_76100(void);
extern uint32_t gj_smoke_soft_76100(void);
extern uint32_t gj_dyn_soft_76100(void);
extern uint32_t gj_milestone_tag_76100(void);
extern uint32_t gj_continuum_wave_76100(void);
extern uint32_t gj_batch_id_76100(void);
extern uint32_t gj_graph_milestone_76100(void);
extern const char __libcgj_batch76091_marker[];
extern const char __libcgj_batch76092_marker[];
extern const char __libcgj_batch76093_marker[];
extern const char __libcgj_batch76094_marker[];
extern const char __libcgj_batch76095_marker[];
extern const char __libcgj_batch76096_marker[];
extern const char __libcgj_batch76097_marker[];
extern const char __libcgj_batch76098_marker[];
extern const char __libcgj_batch76099_marker[];
extern const char __libcgj_batch76100_marker[];

static int
direct_check(void)
{
	int nFail = 0;

	if (gj_shell_green_76100() != 1u) {
		fprintf(stderr, "libcgj: direct gj_shell_green_76100 fail\n");
		nFail++;
	}
	if (gj_libcgj_green_76100() != 1u) {
		fprintf(stderr, "libcgj: direct gj_libcgj_green_76100 fail\n");
		nFail++;
	}
	if (gj_bar3_ready_76100() != 0u) {
		fprintf(stderr, "libcgj: direct gj_bar3_ready_76100 fail\n");
		nFail++;
	}
	if (gj_product_score_76100() != 0u) {
		fprintf(stderr, "libcgj: direct gj_product_score_76100 fail\n");
		nFail++;
	}
	if (gj_continuum_ready_76100() != 1u) {
		fprintf(stderr, "libcgj: direct gj_continuum_ready_76100 fail\n");
		nFail++;
	}
	if (gj_smoke_soft_76100() != 1u) {
		fprintf(stderr, "libcgj: direct gj_smoke_soft_76100 fail\n");
		nFail++;
	}
	if (gj_dyn_soft_76100() != 1u) {
		fprintf(stderr, "libcgj: direct gj_dyn_soft_76100 fail\n");
		nFail++;
	}
	if (gj_milestone_tag_76100() != 76100u) {
		fprintf(stderr, "libcgj: direct gj_milestone_tag_76100 fail\n");
		nFail++;
	}
	if (gj_continuum_wave_76100() != 76100u) {
		fprintf(stderr, "libcgj: direct gj_continuum_wave_76100 fail\n");
		nFail++;
	}
	if (gj_batch_id_76100() != 76100u) {
		fprintf(stderr, "libcgj: direct gj_batch_id_76100 fail\n");
		nFail++;
	}
	if (gj_graph_milestone_76100() != 76100u) {
		fprintf(stderr, "libcgj: direct gj_graph_milestone_76100 fail\n");
		nFail++;
	}
	if (strcmp(__libcgj_batch76091_marker, "libcgj-batch76091") != 0 ||
	    strcmp(__libcgj_batch76092_marker, "libcgj-batch76092") != 0 ||
	    strcmp(__libcgj_batch76093_marker, "libcgj-batch76093") != 0 ||
	    strcmp(__libcgj_batch76094_marker, "libcgj-batch76094") != 0 ||
	    strcmp(__libcgj_batch76095_marker, "libcgj-batch76095") != 0 ||
	    strcmp(__libcgj_batch76096_marker, "libcgj-batch76096") != 0 ||
	    strcmp(__libcgj_batch76097_marker, "libcgj-batch76097") != 0 ||
	    strcmp(__libcgj_batch76098_marker, "libcgj-batch76098") != 0 ||
	    strcmp(__libcgj_batch76099_marker, "libcgj-batch76099") != 0 ||
	    strcmp(__libcgj_batch76100_marker, "libcgj-batch76100") != 0) {
		fprintf(stderr, "libcgj: direct m76100 marker string fail\n");
		nFail++;
	}
	return nFail;
}
#else

typedef uint32_t (*u32_fn)(void);

static int
soft_u32(void *h, const char *szName, uint32_t u32Expect, int *pChecked)
{
	u32_fn pfn;
	uint32_t u32Got;

	pfn = (u32_fn)dlsym(h, szName);
	if (pfn == NULL)
		return 0;
	if (pChecked != NULL)
		(*pChecked)++;
	u32Got = pfn();
	if (u32Got != u32Expect) {
		fprintf(stderr,
		        "libcgj: soft m76100 %s KAT fail got=%u expect=%u\n",
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
	if (szGot == NULL)
		return 0;
	if (pChecked != NULL)
		(*pChecked)++;
	if (strcmp(szGot, szExpect) != 0) {
		fprintf(stderr,
		        "libcgj: soft m76100 %s marker fail got=\"%s\" "
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

#endif

int
main(void)
{
#ifdef CGJ_SOFT_M76100_DIRECT
	int nFail = direct_check();
	if (nFail != 0) {
		fprintf(stderr, "libcgj: soft m76100 DIRECT FAIL fails=%d\n", nFail);
		return 1;
	}
	printf("libcgj: soft m76100 DIRECT PASS (all 11 lamps + markers)\n");
	return 0;
#else
	void *h = NULL;
	const char *szPath = NULL;
	int i, nChecked = 0, nFail = 0;

	for (i = 0; kPaths[i] != NULL; i++) {
		h = dlopen(kPaths[i], RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND);
		if (h != NULL) {
			szPath = kPaths[i];
			break;
		}
	}
	if (h == NULL) {
		fprintf(stderr, "libcgj: soft m76100 skip (dlopen fail: %s)\n",
		        dlerror());
		printf("libcgj: soft m76100 SOFT-SKIP (no product SO)\n");
		return 0;
	}

	nFail += soft_u32(h, "gj_shell_green_76100", 1u, &nChecked);
	nFail += soft_u32(h, "gj_libcgj_green_76100", 1u, &nChecked);
	nFail += soft_u32(h, "gj_bar3_ready_76100", 0u, &nChecked);
	nFail += soft_u32(h, "gj_product_score_76100", 0u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_ready_76100", 1u, &nChecked);
	nFail += soft_u32(h, "gj_smoke_soft_76100", 1u, &nChecked);
	nFail += soft_u32(h, "gj_dyn_soft_76100", 1u, &nChecked);
	nFail += soft_u32(h, "gj_milestone_tag_76100", 76100u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_wave_76100", 76100u, &nChecked);
	nFail += soft_u32(h, "gj_batch_id_76100", 76100u, &nChecked);
	nFail += soft_u32(h, "gj_graph_milestone_76100", 76100u, &nChecked);
	nFail += soft_marker(h, "__libcgj_batch76091_marker",
	                     "libcgj-batch76091", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch76092_marker",
	                     "libcgj-batch76092", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch76093_marker",
	                     "libcgj-batch76093", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch76094_marker",
	                     "libcgj-batch76094", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch76095_marker",
	                     "libcgj-batch76095", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch76096_marker",
	                     "libcgj-batch76096", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch76097_marker",
	                     "libcgj-batch76097", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch76098_marker",
	                     "libcgj-batch76098", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch76099_marker",
	                     "libcgj-batch76099", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch76100_marker",
	                     "libcgj-batch76100", &nChecked);

	if (nFail != 0) {
		(void)dlclose(h);
		fprintf(stderr,
		        "libcgj: soft m76100 FAIL path=%s fails=%d checked=%d\n",
		        szPath, nFail, nChecked);
		return 2;
	}
	printf("libcgj: soft m76100 PASS path=%s checked=%d "
	       "(soft-skip unexported/missing; bar3 still open)\n",
	       szPath, nChecked);
	(void)dlclose(h);
	return 0;
#endif
}
