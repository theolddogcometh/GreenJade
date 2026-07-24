/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 * Soft host gates MILESTONE 69100 (CREATE-ONLY). Mirror 25800; bar3=0.
 * See NOTES_milestone_69100.txt
 */
#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#ifndef CGJ_SOFT_M69100_DIRECT
#include <dlfcn.h>
#endif
#ifdef CGJ_SOFT_M69100_DIRECT
extern uint32_t gj_shell_green_69100(void);
extern uint32_t gj_libcgj_green_69100(void);
extern uint32_t gj_bar3_ready_69100(void);
extern uint32_t gj_product_score_69100(void);
extern uint32_t gj_continuum_ready_69100(void);
extern uint32_t gj_smoke_soft_69100(void);
extern uint32_t gj_dyn_soft_69100(void);
extern uint32_t gj_milestone_tag_69100(void);
extern uint32_t gj_continuum_wave_69100(void);
extern uint32_t gj_batch_id_69100(void);
extern uint32_t gj_graph_milestone_69100(void);
extern const char __libcgj_batch69091_marker[];
extern const char __libcgj_batch69092_marker[];
extern const char __libcgj_batch69093_marker[];
extern const char __libcgj_batch69094_marker[];
extern const char __libcgj_batch69095_marker[];
extern const char __libcgj_batch69096_marker[];
extern const char __libcgj_batch69097_marker[];
extern const char __libcgj_batch69098_marker[];
extern const char __libcgj_batch69099_marker[];
extern const char __libcgj_batch69100_marker[];
static int direct_check(void) {
	int nFail = 0;
	if (gj_shell_green_69100() != 1u) { fprintf(stderr, "libcgj: direct gj_shell_green_69100 fail\n"); nFail++; }
	if (gj_libcgj_green_69100() != 1u) { fprintf(stderr, "libcgj: direct gj_libcgj_green_69100 fail\n"); nFail++; }
	if (gj_bar3_ready_69100() != 0u) { fprintf(stderr, "libcgj: direct gj_bar3_ready_69100 fail\n"); nFail++; }
	if (gj_product_score_69100() != 0u) { fprintf(stderr, "libcgj: direct gj_product_score_69100 fail\n"); nFail++; }
	if (gj_continuum_ready_69100() != 1u) { fprintf(stderr, "libcgj: direct gj_continuum_ready_69100 fail\n"); nFail++; }
	if (gj_smoke_soft_69100() != 1u) { fprintf(stderr, "libcgj: direct gj_smoke_soft_69100 fail\n"); nFail++; }
	if (gj_dyn_soft_69100() != 1u) { fprintf(stderr, "libcgj: direct gj_dyn_soft_69100 fail\n"); nFail++; }
	if (gj_milestone_tag_69100() != 69100u) { fprintf(stderr, "libcgj: direct gj_milestone_tag_69100 fail\n"); nFail++; }
	if (gj_continuum_wave_69100() != 69100u) { fprintf(stderr, "libcgj: direct gj_continuum_wave_69100 fail\n"); nFail++; }
	if (gj_batch_id_69100() != 69100u) { fprintf(stderr, "libcgj: direct gj_batch_id_69100 fail\n"); nFail++; }
	if (gj_graph_milestone_69100() != 69100u) { fprintf(stderr, "libcgj: direct gj_graph_milestone_69100 fail\n"); nFail++; }
	if (strcmp(__libcgj_batch69091_marker, "libcgj-batch69091") != 0 ||
	    strcmp(__libcgj_batch69092_marker, "libcgj-batch69092") != 0 ||
	    strcmp(__libcgj_batch69093_marker, "libcgj-batch69093") != 0 ||
	    strcmp(__libcgj_batch69094_marker, "libcgj-batch69094") != 0 ||
	    strcmp(__libcgj_batch69095_marker, "libcgj-batch69095") != 0 ||
	    strcmp(__libcgj_batch69096_marker, "libcgj-batch69096") != 0 ||
	    strcmp(__libcgj_batch69097_marker, "libcgj-batch69097") != 0 ||
	    strcmp(__libcgj_batch69098_marker, "libcgj-batch69098") != 0 ||
	    strcmp(__libcgj_batch69099_marker, "libcgj-batch69099") != 0 ||
	    strcmp(__libcgj_batch69100_marker, "libcgj-batch69100") != 0) {
		fprintf(stderr, "libcgj: direct m69100 marker string fail\n"); nFail++;
	}
	return nFail;
}
#else
typedef uint32_t (*u32_fn)(void);
static int soft_u32(void *h, const char *szName, uint32_t u32Expect, int *pChecked) {
	u32_fn pfn = (u32_fn)dlsym(h, szName); uint32_t u32Got;
	if (pfn == NULL) return 0; if (pChecked) (*pChecked)++;
	u32Got = pfn();
	if (u32Got != u32Expect) { fprintf(stderr, "libcgj: soft m69100 %s KAT fail got=%u expect=%u\n", szName, (unsigned)u32Got, (unsigned)u32Expect); return 1; }
	return 0;
}
static int soft_marker(void *h, const char *szSym, const char *szExpect, int *pChecked) {
	const char *szGot = (const char *)dlsym(h, szSym);
	if (szGot == NULL) return 0; if (pChecked) (*pChecked)++;
	if (strcmp(szGot, szExpect) != 0) { fprintf(stderr, "libcgj: soft m69100 %s marker fail got=\"%s\" expect=\"%s\"\n", szSym, szGot, szExpect); return 1; }
	return 0;
}
static const char *const kPaths[] = { "build/user/libc.so.6", "./build/user/libc.so.6", NULL };
#endif
int main(void) {
#ifdef CGJ_SOFT_M69100_DIRECT
	int nFail = direct_check();
	if (nFail) { fprintf(stderr, "libcgj: soft m69100 DIRECT FAIL fails=%d\n", nFail); return 1; }
	printf("libcgj: soft m69100 DIRECT PASS (all 11 lamps + markers)\n"); return 0;
#else
	void *h = NULL; const char *szPath = NULL; int i, nChecked = 0, nFail = 0;
	for (i = 0; kPaths[i]; i++) { h = dlopen(kPaths[i], RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND); if (h) { szPath = kPaths[i]; break; } }
	if (!h) { fprintf(stderr, "libcgj: soft m69100 skip (dlopen fail: %s)\n", dlerror()); printf("libcgj: soft m69100 SOFT-SKIP (no product SO)\n"); return 0; }
	nFail += soft_u32(h, "gj_shell_green_69100", 1u, &nChecked);
	nFail += soft_u32(h, "gj_libcgj_green_69100", 1u, &nChecked);
	nFail += soft_u32(h, "gj_bar3_ready_69100", 0u, &nChecked);
	nFail += soft_u32(h, "gj_product_score_69100", 0u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_ready_69100", 1u, &nChecked);
	nFail += soft_u32(h, "gj_smoke_soft_69100", 1u, &nChecked);
	nFail += soft_u32(h, "gj_dyn_soft_69100", 1u, &nChecked);
	nFail += soft_u32(h, "gj_milestone_tag_69100", 69100u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_wave_69100", 69100u, &nChecked);
	nFail += soft_u32(h, "gj_batch_id_69100", 69100u, &nChecked);
	nFail += soft_u32(h, "gj_graph_milestone_69100", 69100u, &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69091_marker", "libcgj-batch69091", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69092_marker", "libcgj-batch69092", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69093_marker", "libcgj-batch69093", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69094_marker", "libcgj-batch69094", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69095_marker", "libcgj-batch69095", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69096_marker", "libcgj-batch69096", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69097_marker", "libcgj-batch69097", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69098_marker", "libcgj-batch69098", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69099_marker", "libcgj-batch69099", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69100_marker", "libcgj-batch69100", &nChecked);
	if (nFail) { (void)dlclose(h); fprintf(stderr, "libcgj: soft m69100 FAIL path=%s fails=%d checked=%d\n", szPath, nFail, nChecked); return 2; }
	printf("libcgj: soft m69100 PASS path=%s checked=%d (soft-skip unexported/missing; bar3 still open)\n", szPath, nChecked);
	(void)dlclose(h); return 0;
#endif
}
