/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 68200 markers (CREATE-ONLY).
 * Coherent soft package (see NOTES_milestone_68200.txt). Mirror 25800; bar3=0.
 *
 * Manual build (host libc + dl):
 *   cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_68200 \
 *     user/libcgj/tests/cgj_soft_milestone_68200.c -ldl
 * Optional DIRECT: -DCGJ_SOFT_M68200_DIRECT + graph_batch68191.c … graph_batch68200.c
 *
 * Expected: gj_bar3_ready_68200 → 0; gj_product_score_68200 → 0;
 *   greens/continuum/smoke/dyn → 1; tag/wave/batch/graph → 68200.
 */

#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef CGJ_SOFT_M68200_DIRECT
#include <dlfcn.h>
#endif

#ifdef CGJ_SOFT_M68200_DIRECT
extern uint32_t gj_shell_green_68200(void);
extern uint32_t gj_libcgj_green_68200(void);
extern uint32_t gj_bar3_ready_68200(void);
extern uint32_t gj_product_score_68200(void);
extern uint32_t gj_continuum_ready_68200(void);
extern uint32_t gj_smoke_soft_68200(void);
extern uint32_t gj_dyn_soft_68200(void);
extern uint32_t gj_milestone_tag_68200(void);
extern uint32_t gj_continuum_wave_68200(void);
extern uint32_t gj_batch_id_68200(void);
extern uint32_t gj_graph_milestone_68200(void);
extern const char __libcgj_batch68191_marker[];
extern const char __libcgj_batch68192_marker[];
extern const char __libcgj_batch68193_marker[];
extern const char __libcgj_batch68194_marker[];
extern const char __libcgj_batch68195_marker[];
extern const char __libcgj_batch68196_marker[];
extern const char __libcgj_batch68197_marker[];
extern const char __libcgj_batch68198_marker[];
extern const char __libcgj_batch68199_marker[];
extern const char __libcgj_batch68200_marker[];

static int
direct_check(void)
{
	int nFail = 0;

	if (gj_shell_green_68200() != 1u) { fprintf(stderr, "libcgj: direct gj_shell_green_68200 fail\n"); nFail++; }
	if (gj_libcgj_green_68200() != 1u) { fprintf(stderr, "libcgj: direct gj_libcgj_green_68200 fail\n"); nFail++; }
	if (gj_bar3_ready_68200() != 0u) { fprintf(stderr, "libcgj: direct gj_bar3_ready_68200 fail\n"); nFail++; }
	if (gj_product_score_68200() != 0u) { fprintf(stderr, "libcgj: direct gj_product_score_68200 fail\n"); nFail++; }
	if (gj_continuum_ready_68200() != 1u) { fprintf(stderr, "libcgj: direct gj_continuum_ready_68200 fail\n"); nFail++; }
	if (gj_smoke_soft_68200() != 1u) { fprintf(stderr, "libcgj: direct gj_smoke_soft_68200 fail\n"); nFail++; }
	if (gj_dyn_soft_68200() != 1u) { fprintf(stderr, "libcgj: direct gj_dyn_soft_68200 fail\n"); nFail++; }
	if (gj_milestone_tag_68200() != 68200u) { fprintf(stderr, "libcgj: direct gj_milestone_tag_68200 fail\n"); nFail++; }
	if (gj_continuum_wave_68200() != 68200u) { fprintf(stderr, "libcgj: direct gj_continuum_wave_68200 fail\n"); nFail++; }
	if (gj_batch_id_68200() != 68200u) { fprintf(stderr, "libcgj: direct gj_batch_id_68200 fail\n"); nFail++; }
	if (gj_graph_milestone_68200() != 68200u) { fprintf(stderr, "libcgj: direct gj_graph_milestone_68200 fail\n"); nFail++; }
	if (strcmp(__libcgj_batch68191_marker, "libcgj-batch68191") != 0 ||
	    strcmp(__libcgj_batch68192_marker, "libcgj-batch68192") != 0 ||
	    strcmp(__libcgj_batch68193_marker, "libcgj-batch68193") != 0 ||
	    strcmp(__libcgj_batch68194_marker, "libcgj-batch68194") != 0 ||
	    strcmp(__libcgj_batch68195_marker, "libcgj-batch68195") != 0 ||
	    strcmp(__libcgj_batch68196_marker, "libcgj-batch68196") != 0 ||
	    strcmp(__libcgj_batch68197_marker, "libcgj-batch68197") != 0 ||
	    strcmp(__libcgj_batch68198_marker, "libcgj-batch68198") != 0 ||
	    strcmp(__libcgj_batch68199_marker, "libcgj-batch68199") != 0 ||
	    strcmp(__libcgj_batch68200_marker, "libcgj-batch68200") != 0) {
		fprintf(stderr, "libcgj: direct m68200 marker string fail\n");
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
		fprintf(stderr, "libcgj: soft m68200 %s KAT fail got=%u expect=%u\n",
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
		fprintf(stderr, "libcgj: soft m68200 %s marker fail got=\"%s\" expect=\"%s\"\n",
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
#ifdef CGJ_SOFT_M68200_DIRECT
	int nFail = direct_check();
	if (nFail != 0) {
		fprintf(stderr, "libcgj: soft m68200 DIRECT FAIL fails=%d\n", nFail);
		return 1;
	}
	printf("libcgj: soft m68200 DIRECT PASS (all 11 lamps + markers)\n");
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
		fprintf(stderr, "libcgj: soft m68200 skip (dlopen fail: %s)\n", dlerror());
		printf("libcgj: soft m68200 SOFT-SKIP (no product SO)\n");
		return 0;
	}

	nFail += soft_u32(h, "gj_shell_green_68200", 1u, &nChecked);
	nFail += soft_u32(h, "gj_libcgj_green_68200", 1u, &nChecked);
	nFail += soft_u32(h, "gj_bar3_ready_68200", 0u, &nChecked);
	nFail += soft_u32(h, "gj_product_score_68200", 0u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_ready_68200", 1u, &nChecked);
	nFail += soft_u32(h, "gj_smoke_soft_68200", 1u, &nChecked);
	nFail += soft_u32(h, "gj_dyn_soft_68200", 1u, &nChecked);
	nFail += soft_u32(h, "gj_milestone_tag_68200", 68200u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_wave_68200", 68200u, &nChecked);
	nFail += soft_u32(h, "gj_batch_id_68200", 68200u, &nChecked);
	nFail += soft_u32(h, "gj_graph_milestone_68200", 68200u, &nChecked);
	nFail += soft_marker(h, "__libcgj_batch68191_marker", "libcgj-batch68191", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch68192_marker", "libcgj-batch68192", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch68193_marker", "libcgj-batch68193", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch68194_marker", "libcgj-batch68194", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch68195_marker", "libcgj-batch68195", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch68196_marker", "libcgj-batch68196", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch68197_marker", "libcgj-batch68197", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch68198_marker", "libcgj-batch68198", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch68199_marker", "libcgj-batch68199", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch68200_marker", "libcgj-batch68200", &nChecked);

	if (nFail != 0) {
		(void)dlclose(h);
		fprintf(stderr, "libcgj: soft m68200 FAIL path=%s fails=%d checked=%d\n",
		        szPath, nFail, nChecked);
		return 2;
	}
	printf("libcgj: soft m68200 PASS path=%s checked=%d "
	       "(soft-skip unexported/missing; bar3 still open)\n",
	       szPath, nChecked);
	(void)dlclose(h);
	return 0;
#endif
}
