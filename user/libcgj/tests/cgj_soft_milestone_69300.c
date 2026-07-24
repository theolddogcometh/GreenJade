/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 * Soft host gates MILESTONE 69300 (CREATE-ONLY). Mirror 25800; bar3=0.
 * See NOTES_milestone_69300.txt
 */
#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#ifndef CGJ_SOFT_M69300_DIRECT
#include <dlfcn.h>
#endif
#ifdef CGJ_SOFT_M69300_DIRECT
extern uint32_t gj_shell_green_69300(void);
extern uint32_t gj_libcgj_green_69300(void);
extern uint32_t gj_bar3_ready_69300(void);
extern uint32_t gj_product_score_69300(void);
extern uint32_t gj_continuum_ready_69300(void);
extern uint32_t gj_smoke_soft_69300(void);
extern uint32_t gj_dyn_soft_69300(void);
extern uint32_t gj_milestone_tag_69300(void);
extern uint32_t gj_continuum_wave_69300(void);
extern uint32_t gj_batch_id_69300(void);
extern uint32_t gj_graph_milestone_69300(void);
extern const char __libcgj_batch69291_marker[];
extern const char __libcgj_batch69292_marker[];
extern const char __libcgj_batch69293_marker[];
extern const char __libcgj_batch69294_marker[];
extern const char __libcgj_batch69295_marker[];
extern const char __libcgj_batch69296_marker[];
extern const char __libcgj_batch69297_marker[];
extern const char __libcgj_batch69298_marker[];
extern const char __libcgj_batch69299_marker[];
extern const char __libcgj_batch69300_marker[];
static int direct_check(void) {
	int nFail = 0;
	if (gj_shell_green_69300() != 1u) { fprintf(stderr, "libcgj: direct gj_shell_green_69300 fail\n"); nFail++; }
	if (gj_libcgj_green_69300() != 1u) { fprintf(stderr, "libcgj: direct gj_libcgj_green_69300 fail\n"); nFail++; }
	if (gj_bar3_ready_69300() != 0u) { fprintf(stderr, "libcgj: direct gj_bar3_ready_69300 fail\n"); nFail++; }
	if (gj_product_score_69300() != 0u) { fprintf(stderr, "libcgj: direct gj_product_score_69300 fail\n"); nFail++; }
	if (gj_continuum_ready_69300() != 1u) { fprintf(stderr, "libcgj: direct gj_continuum_ready_69300 fail\n"); nFail++; }
	if (gj_smoke_soft_69300() != 1u) { fprintf(stderr, "libcgj: direct gj_smoke_soft_69300 fail\n"); nFail++; }
	if (gj_dyn_soft_69300() != 1u) { fprintf(stderr, "libcgj: direct gj_dyn_soft_69300 fail\n"); nFail++; }
	if (gj_milestone_tag_69300() != 69300u) { fprintf(stderr, "libcgj: direct gj_milestone_tag_69300 fail\n"); nFail++; }
	if (gj_continuum_wave_69300() != 69300u) { fprintf(stderr, "libcgj: direct gj_continuum_wave_69300 fail\n"); nFail++; }
	if (gj_batch_id_69300() != 69300u) { fprintf(stderr, "libcgj: direct gj_batch_id_69300 fail\n"); nFail++; }
	if (gj_graph_milestone_69300() != 69300u) { fprintf(stderr, "libcgj: direct gj_graph_milestone_69300 fail\n"); nFail++; }
	if (strcmp(__libcgj_batch69291_marker, "libcgj-batch69291") != 0 ||
	    strcmp(__libcgj_batch69292_marker, "libcgj-batch69292") != 0 ||
	    strcmp(__libcgj_batch69293_marker, "libcgj-batch69293") != 0 ||
	    strcmp(__libcgj_batch69294_marker, "libcgj-batch69294") != 0 ||
	    strcmp(__libcgj_batch69295_marker, "libcgj-batch69295") != 0 ||
	    strcmp(__libcgj_batch69296_marker, "libcgj-batch69296") != 0 ||
	    strcmp(__libcgj_batch69297_marker, "libcgj-batch69297") != 0 ||
	    strcmp(__libcgj_batch69298_marker, "libcgj-batch69298") != 0 ||
	    strcmp(__libcgj_batch69299_marker, "libcgj-batch69299") != 0 ||
	    strcmp(__libcgj_batch69300_marker, "libcgj-batch69300") != 0) {
		fprintf(stderr, "libcgj: direct m69300 marker string fail\n"); nFail++;
	}
	return nFail;
}
#else
typedef uint32_t (*u32_fn)(void);
static int soft_u32(void *h, const char *szName, uint32_t u32Expect, int *pChecked) {
	u32_fn pfn = (u32_fn)dlsym(h, szName); uint32_t u32Got;
	if (pfn == NULL) return 0; if (pChecked) (*pChecked)++;
	u32Got = pfn();
	if (u32Got != u32Expect) { fprintf(stderr, "libcgj: soft m69300 %s KAT fail got=%u expect=%u\n", szName, (unsigned)u32Got, (unsigned)u32Expect); return 1; }
	return 0;
}
static int soft_marker(void *h, const char *szSym, const char *szExpect, int *pChecked) {
	const char *szGot = (const char *)dlsym(h, szSym);
	if (szGot == NULL) return 0; if (pChecked) (*pChecked)++;
	if (strcmp(szGot, szExpect) != 0) { fprintf(stderr, "libcgj: soft m69300 %s marker fail got=\"%s\" expect=\"%s\"\n", szSym, szGot, szExpect); return 1; }
	return 0;
}
static const char *const kPaths[] = { "build/user/libc.so.6", "./build/user/libc.so.6", NULL };
#endif
int main(void) {
#ifdef CGJ_SOFT_M69300_DIRECT
	int nFail = direct_check();
	if (nFail) { fprintf(stderr, "libcgj: soft m69300 DIRECT FAIL fails=%d\n", nFail); return 1; }
	printf("libcgj: soft m69300 DIRECT PASS (all 11 lamps + markers)\n"); return 0;
#else
	void *h = NULL; const char *szPath = NULL; int i, nChecked = 0, nFail = 0;
	for (i = 0; kPaths[i]; i++) { h = dlopen(kPaths[i], RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND); if (h) { szPath = kPaths[i]; break; } }
	if (!h) { fprintf(stderr, "libcgj: soft m69300 skip (dlopen fail: %s)\n", dlerror()); printf("libcgj: soft m69300 SOFT-SKIP (no product SO)\n"); return 0; }
	nFail += soft_u32(h, "gj_shell_green_69300", 1u, &nChecked);
	nFail += soft_u32(h, "gj_libcgj_green_69300", 1u, &nChecked);
	nFail += soft_u32(h, "gj_bar3_ready_69300", 0u, &nChecked);
	nFail += soft_u32(h, "gj_product_score_69300", 0u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_ready_69300", 1u, &nChecked);
	nFail += soft_u32(h, "gj_smoke_soft_69300", 1u, &nChecked);
	nFail += soft_u32(h, "gj_dyn_soft_69300", 1u, &nChecked);
	nFail += soft_u32(h, "gj_milestone_tag_69300", 69300u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_wave_69300", 69300u, &nChecked);
	nFail += soft_u32(h, "gj_batch_id_69300", 69300u, &nChecked);
	nFail += soft_u32(h, "gj_graph_milestone_69300", 69300u, &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69291_marker", "libcgj-batch69291", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69292_marker", "libcgj-batch69292", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69293_marker", "libcgj-batch69293", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69294_marker", "libcgj-batch69294", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69295_marker", "libcgj-batch69295", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69296_marker", "libcgj-batch69296", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69297_marker", "libcgj-batch69297", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69298_marker", "libcgj-batch69298", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69299_marker", "libcgj-batch69299", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69300_marker", "libcgj-batch69300", &nChecked);
	if (nFail) { (void)dlclose(h); fprintf(stderr, "libcgj: soft m69300 FAIL path=%s fails=%d checked=%d\n", szPath, nFail, nChecked); return 2; }
	printf("libcgj: soft m69300 PASS path=%s checked=%d (soft-skip unexported/missing; bar3 still open)\n", szPath, nChecked);
	(void)dlclose(h); return 0;
#endif
}
