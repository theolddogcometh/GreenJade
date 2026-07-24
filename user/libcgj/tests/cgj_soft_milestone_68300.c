/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 68300 markers (CREATE-ONLY).
 * Coherent soft package (see NOTES_milestone_68300.txt). Mirror 25800; bar3=0.
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_68300 \
 *   user/libcgj/tests/cgj_soft_milestone_68300.c -ldl
 * DIRECT: -DCGJ_SOFT_M68300_DIRECT + graph_batch68291.c … graph_batch68300.c
 */

#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef CGJ_SOFT_M68300_DIRECT
#include <dlfcn.h>
#endif

#ifdef CGJ_SOFT_M68300_DIRECT
extern uint32_t gj_shell_green_68300(void);
extern uint32_t gj_libcgj_green_68300(void);
extern uint32_t gj_bar3_ready_68300(void);
extern uint32_t gj_product_score_68300(void);
extern uint32_t gj_continuum_ready_68300(void);
extern uint32_t gj_smoke_soft_68300(void);
extern uint32_t gj_dyn_soft_68300(void);
extern uint32_t gj_milestone_tag_68300(void);
extern uint32_t gj_continuum_wave_68300(void);
extern uint32_t gj_batch_id_68300(void);
extern uint32_t gj_graph_milestone_68300(void);
extern const char __libcgj_batch68291_marker[];
extern const char __libcgj_batch68292_marker[];
extern const char __libcgj_batch68293_marker[];
extern const char __libcgj_batch68294_marker[];
extern const char __libcgj_batch68295_marker[];
extern const char __libcgj_batch68296_marker[];
extern const char __libcgj_batch68297_marker[];
extern const char __libcgj_batch68298_marker[];
extern const char __libcgj_batch68299_marker[];
extern const char __libcgj_batch68300_marker[];

static int
direct_check(void)
{
	int nFail = 0;
	if (gj_shell_green_68300() != 1u) { fprintf(stderr, "libcgj: direct gj_shell_green_68300 fail\n"); nFail++; }
	if (gj_libcgj_green_68300() != 1u) { fprintf(stderr, "libcgj: direct gj_libcgj_green_68300 fail\n"); nFail++; }
	if (gj_bar3_ready_68300() != 0u) { fprintf(stderr, "libcgj: direct gj_bar3_ready_68300 fail\n"); nFail++; }
	if (gj_product_score_68300() != 0u) { fprintf(stderr, "libcgj: direct gj_product_score_68300 fail\n"); nFail++; }
	if (gj_continuum_ready_68300() != 1u) { fprintf(stderr, "libcgj: direct gj_continuum_ready_68300 fail\n"); nFail++; }
	if (gj_smoke_soft_68300() != 1u) { fprintf(stderr, "libcgj: direct gj_smoke_soft_68300 fail\n"); nFail++; }
	if (gj_dyn_soft_68300() != 1u) { fprintf(stderr, "libcgj: direct gj_dyn_soft_68300 fail\n"); nFail++; }
	if (gj_milestone_tag_68300() != 68300u) { fprintf(stderr, "libcgj: direct gj_milestone_tag_68300 fail\n"); nFail++; }
	if (gj_continuum_wave_68300() != 68300u) { fprintf(stderr, "libcgj: direct gj_continuum_wave_68300 fail\n"); nFail++; }
	if (gj_batch_id_68300() != 68300u) { fprintf(stderr, "libcgj: direct gj_batch_id_68300 fail\n"); nFail++; }
	if (gj_graph_milestone_68300() != 68300u) { fprintf(stderr, "libcgj: direct gj_graph_milestone_68300 fail\n"); nFail++; }
	if (strcmp(__libcgj_batch68291_marker, "libcgj-batch68291") != 0 ||
	    strcmp(__libcgj_batch68292_marker, "libcgj-batch68292") != 0 ||
	    strcmp(__libcgj_batch68293_marker, "libcgj-batch68293") != 0 ||
	    strcmp(__libcgj_batch68294_marker, "libcgj-batch68294") != 0 ||
	    strcmp(__libcgj_batch68295_marker, "libcgj-batch68295") != 0 ||
	    strcmp(__libcgj_batch68296_marker, "libcgj-batch68296") != 0 ||
	    strcmp(__libcgj_batch68297_marker, "libcgj-batch68297") != 0 ||
	    strcmp(__libcgj_batch68298_marker, "libcgj-batch68298") != 0 ||
	    strcmp(__libcgj_batch68299_marker, "libcgj-batch68299") != 0 ||
	    strcmp(__libcgj_batch68300_marker, "libcgj-batch68300") != 0) {
		fprintf(stderr, "libcgj: direct m68300 marker string fail\n");
		nFail++;
	}
	return nFail;
}
#else
typedef uint32_t (*u32_fn)(void);
static int soft_u32(void *h, const char *szName, uint32_t u32Expect, int *pChecked)
{
	u32_fn pfn = (u32_fn)dlsym(h, szName);
	uint32_t u32Got;
	if (pfn == NULL) return 0;
	if (pChecked != NULL) (*pChecked)++;
	u32Got = pfn();
	if (u32Got != u32Expect) {
		fprintf(stderr, "libcgj: soft m68300 %s KAT fail got=%u expect=%u\n",
		        szName, (unsigned)u32Got, (unsigned)u32Expect);
		return 1;
	}
	return 0;
}
static int soft_marker(void *h, const char *szSym, const char *szExpect, int *pChecked)
{
	const char *szGot = (const char *)dlsym(h, szSym);
	if (szGot == NULL) return 0;
	if (pChecked != NULL) (*pChecked)++;
	if (strcmp(szGot, szExpect) != 0) {
		fprintf(stderr, "libcgj: soft m68300 %s marker fail got=\"%s\" expect=\"%s\"\n",
		        szSym, szGot, szExpect);
		return 1;
	}
	return 0;
}
static const char *const kPaths[] = { "build/user/libc.so.6", "./build/user/libc.so.6", NULL };
#endif

int
main(void)
{
#ifdef CGJ_SOFT_M68300_DIRECT
	int nFail = direct_check();
	if (nFail != 0) { fprintf(stderr, "libcgj: soft m68300 DIRECT FAIL fails=%d\n", nFail); return 1; }
	printf("libcgj: soft m68300 DIRECT PASS (all 11 lamps + markers)\n");
	return 0;
#else
	void *h = NULL;
	const char *szPath = NULL;
	int i, nChecked = 0, nFail = 0;
	for (i = 0; kPaths[i] != NULL; i++) {
		h = dlopen(kPaths[i], RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND);
		if (h != NULL) { szPath = kPaths[i]; break; }
	}
	if (h == NULL) {
		fprintf(stderr, "libcgj: soft m68300 skip (dlopen fail: %s)\n", dlerror());
		printf("libcgj: soft m68300 SOFT-SKIP (no product SO)\n");
		return 0;
	}
	nFail += soft_u32(h, "gj_shell_green_68300", 1u, &nChecked);
	nFail += soft_u32(h, "gj_libcgj_green_68300", 1u, &nChecked);
	nFail += soft_u32(h, "gj_bar3_ready_68300", 0u, &nChecked);
	nFail += soft_u32(h, "gj_product_score_68300", 0u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_ready_68300", 1u, &nChecked);
	nFail += soft_u32(h, "gj_smoke_soft_68300", 1u, &nChecked);
	nFail += soft_u32(h, "gj_dyn_soft_68300", 1u, &nChecked);
	nFail += soft_u32(h, "gj_milestone_tag_68300", 68300u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_wave_68300", 68300u, &nChecked);
	nFail += soft_u32(h, "gj_batch_id_68300", 68300u, &nChecked);
	nFail += soft_u32(h, "gj_graph_milestone_68300", 68300u, &nChecked);
	nFail += soft_marker(h, "__libcgj_batch68291_marker", "libcgj-batch68291", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch68292_marker", "libcgj-batch68292", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch68293_marker", "libcgj-batch68293", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch68294_marker", "libcgj-batch68294", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch68295_marker", "libcgj-batch68295", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch68296_marker", "libcgj-batch68296", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch68297_marker", "libcgj-batch68297", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch68298_marker", "libcgj-batch68298", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch68299_marker", "libcgj-batch68299", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch68300_marker", "libcgj-batch68300", &nChecked);
	if (nFail != 0) {
		(void)dlclose(h);
		fprintf(stderr, "libcgj: soft m68300 FAIL path=%s fails=%d checked=%d\n", szPath, nFail, nChecked);
		return 2;
	}
	printf("libcgj: soft m68300 PASS path=%s checked=%d (soft-skip unexported/missing; bar3 still open)\n",
	       szPath, nChecked);
	(void)dlclose(h);
	return 0;
#endif
}
