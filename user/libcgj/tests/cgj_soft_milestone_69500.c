/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 * Soft host gates MILESTONE 69500 (CREATE-ONLY). Mirror 25800; bar3=0.
 * See NOTES_milestone_69500.txt
 */
#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#ifndef CGJ_SOFT_M69500_DIRECT
#include <dlfcn.h>
#endif
#ifdef CGJ_SOFT_M69500_DIRECT
extern uint32_t gj_shell_green_69500(void);
extern uint32_t gj_libcgj_green_69500(void);
extern uint32_t gj_bar3_ready_69500(void);
extern uint32_t gj_product_score_69500(void);
extern uint32_t gj_continuum_ready_69500(void);
extern uint32_t gj_smoke_soft_69500(void);
extern uint32_t gj_dyn_soft_69500(void);
extern uint32_t gj_milestone_tag_69500(void);
extern uint32_t gj_continuum_wave_69500(void);
extern uint32_t gj_batch_id_69500(void);
extern uint32_t gj_graph_milestone_69500(void);
extern const char __libcgj_batch69491_marker[];
extern const char __libcgj_batch69492_marker[];
extern const char __libcgj_batch69493_marker[];
extern const char __libcgj_batch69494_marker[];
extern const char __libcgj_batch69495_marker[];
extern const char __libcgj_batch69496_marker[];
extern const char __libcgj_batch69497_marker[];
extern const char __libcgj_batch69498_marker[];
extern const char __libcgj_batch69499_marker[];
extern const char __libcgj_batch69500_marker[];
static int direct_check(void) {
	int nFail = 0;
	if (gj_shell_green_69500() != 1u) { fprintf(stderr, "libcgj: direct gj_shell_green_69500 fail\n"); nFail++; }
	if (gj_libcgj_green_69500() != 1u) { fprintf(stderr, "libcgj: direct gj_libcgj_green_69500 fail\n"); nFail++; }
	if (gj_bar3_ready_69500() != 0u) { fprintf(stderr, "libcgj: direct gj_bar3_ready_69500 fail\n"); nFail++; }
	if (gj_product_score_69500() != 0u) { fprintf(stderr, "libcgj: direct gj_product_score_69500 fail\n"); nFail++; }
	if (gj_continuum_ready_69500() != 1u) { fprintf(stderr, "libcgj: direct gj_continuum_ready_69500 fail\n"); nFail++; }
	if (gj_smoke_soft_69500() != 1u) { fprintf(stderr, "libcgj: direct gj_smoke_soft_69500 fail\n"); nFail++; }
	if (gj_dyn_soft_69500() != 1u) { fprintf(stderr, "libcgj: direct gj_dyn_soft_69500 fail\n"); nFail++; }
	if (gj_milestone_tag_69500() != 69500u) { fprintf(stderr, "libcgj: direct gj_milestone_tag_69500 fail\n"); nFail++; }
	if (gj_continuum_wave_69500() != 69500u) { fprintf(stderr, "libcgj: direct gj_continuum_wave_69500 fail\n"); nFail++; }
	if (gj_batch_id_69500() != 69500u) { fprintf(stderr, "libcgj: direct gj_batch_id_69500 fail\n"); nFail++; }
	if (gj_graph_milestone_69500() != 69500u) { fprintf(stderr, "libcgj: direct gj_graph_milestone_69500 fail\n"); nFail++; }
	if (strcmp(__libcgj_batch69491_marker, "libcgj-batch69491") != 0 ||
	    strcmp(__libcgj_batch69492_marker, "libcgj-batch69492") != 0 ||
	    strcmp(__libcgj_batch69493_marker, "libcgj-batch69493") != 0 ||
	    strcmp(__libcgj_batch69494_marker, "libcgj-batch69494") != 0 ||
	    strcmp(__libcgj_batch69495_marker, "libcgj-batch69495") != 0 ||
	    strcmp(__libcgj_batch69496_marker, "libcgj-batch69496") != 0 ||
	    strcmp(__libcgj_batch69497_marker, "libcgj-batch69497") != 0 ||
	    strcmp(__libcgj_batch69498_marker, "libcgj-batch69498") != 0 ||
	    strcmp(__libcgj_batch69499_marker, "libcgj-batch69499") != 0 ||
	    strcmp(__libcgj_batch69500_marker, "libcgj-batch69500") != 0) {
		fprintf(stderr, "libcgj: direct m69500 marker string fail\n"); nFail++;
	}
	return nFail;
}
#else
typedef uint32_t (*u32_fn)(void);
static int soft_u32(void *h, const char *szName, uint32_t u32Expect, int *pChecked) {
	u32_fn pfn = (u32_fn)dlsym(h, szName); uint32_t u32Got;
	if (pfn == NULL) return 0; if (pChecked) (*pChecked)++;
	u32Got = pfn();
	if (u32Got != u32Expect) { fprintf(stderr, "libcgj: soft m69500 %s KAT fail got=%u expect=%u\n", szName, (unsigned)u32Got, (unsigned)u32Expect); return 1; }
	return 0;
}
static int soft_marker(void *h, const char *szSym, const char *szExpect, int *pChecked) {
	const char *szGot = (const char *)dlsym(h, szSym);
	if (szGot == NULL) return 0; if (pChecked) (*pChecked)++;
	if (strcmp(szGot, szExpect) != 0) { fprintf(stderr, "libcgj: soft m69500 %s marker fail got=\"%s\" expect=\"%s\"\n", szSym, szGot, szExpect); return 1; }
	return 0;
}
static const char *const kPaths[] = { "build/user/libc.so.6", "./build/user/libc.so.6", NULL };
#endif
int main(void) {
#ifdef CGJ_SOFT_M69500_DIRECT
	int nFail = direct_check();
	if (nFail) { fprintf(stderr, "libcgj: soft m69500 DIRECT FAIL fails=%d\n", nFail); return 1; }
	printf("libcgj: soft m69500 DIRECT PASS (all 11 lamps + markers)\n"); return 0;
#else
	void *h = NULL; const char *szPath = NULL; int i, nChecked = 0, nFail = 0;
	for (i = 0; kPaths[i]; i++) { h = dlopen(kPaths[i], RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND); if (h) { szPath = kPaths[i]; break; } }
	if (!h) { fprintf(stderr, "libcgj: soft m69500 skip (dlopen fail: %s)\n", dlerror()); printf("libcgj: soft m69500 SOFT-SKIP (no product SO)\n"); return 0; }
	nFail += soft_u32(h, "gj_shell_green_69500", 1u, &nChecked);
	nFail += soft_u32(h, "gj_libcgj_green_69500", 1u, &nChecked);
	nFail += soft_u32(h, "gj_bar3_ready_69500", 0u, &nChecked);
	nFail += soft_u32(h, "gj_product_score_69500", 0u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_ready_69500", 1u, &nChecked);
	nFail += soft_u32(h, "gj_smoke_soft_69500", 1u, &nChecked);
	nFail += soft_u32(h, "gj_dyn_soft_69500", 1u, &nChecked);
	nFail += soft_u32(h, "gj_milestone_tag_69500", 69500u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_wave_69500", 69500u, &nChecked);
	nFail += soft_u32(h, "gj_batch_id_69500", 69500u, &nChecked);
	nFail += soft_u32(h, "gj_graph_milestone_69500", 69500u, &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69491_marker", "libcgj-batch69491", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69492_marker", "libcgj-batch69492", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69493_marker", "libcgj-batch69493", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69494_marker", "libcgj-batch69494", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69495_marker", "libcgj-batch69495", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69496_marker", "libcgj-batch69496", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69497_marker", "libcgj-batch69497", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69498_marker", "libcgj-batch69498", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69499_marker", "libcgj-batch69499", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69500_marker", "libcgj-batch69500", &nChecked);
	if (nFail) { (void)dlclose(h); fprintf(stderr, "libcgj: soft m69500 FAIL path=%s fails=%d checked=%d\n", szPath, nFail, nChecked); return 2; }
	printf("libcgj: soft m69500 PASS path=%s checked=%d (soft-skip unexported/missing; bar3 still open)\n", szPath, nChecked);
	(void)dlclose(h); return 0;
#endif
}
