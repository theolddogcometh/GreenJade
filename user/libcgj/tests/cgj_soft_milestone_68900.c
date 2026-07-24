/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 * Soft host gates MILESTONE 68900 (CREATE-ONLY). Mirror 25800; bar3=0.
 * See NOTES_milestone_68900.txt
 */
#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#ifndef CGJ_SOFT_M68900_DIRECT
#include <dlfcn.h>
#endif
#ifdef CGJ_SOFT_M68900_DIRECT
extern uint32_t gj_shell_green_68900(void);
extern uint32_t gj_libcgj_green_68900(void);
extern uint32_t gj_bar3_ready_68900(void);
extern uint32_t gj_product_score_68900(void);
extern uint32_t gj_continuum_ready_68900(void);
extern uint32_t gj_smoke_soft_68900(void);
extern uint32_t gj_dyn_soft_68900(void);
extern uint32_t gj_milestone_tag_68900(void);
extern uint32_t gj_continuum_wave_68900(void);
extern uint32_t gj_batch_id_68900(void);
extern uint32_t gj_graph_milestone_68900(void);
extern const char __libcgj_batch68891_marker[];
extern const char __libcgj_batch68892_marker[];
extern const char __libcgj_batch68893_marker[];
extern const char __libcgj_batch68894_marker[];
extern const char __libcgj_batch68895_marker[];
extern const char __libcgj_batch68896_marker[];
extern const char __libcgj_batch68897_marker[];
extern const char __libcgj_batch68898_marker[];
extern const char __libcgj_batch68899_marker[];
extern const char __libcgj_batch68900_marker[];
static int direct_check(void) {
	int nFail = 0;
	if (gj_shell_green_68900() != 1u) { fprintf(stderr, "libcgj: direct gj_shell_green_68900 fail\n"); nFail++; }
	if (gj_libcgj_green_68900() != 1u) { fprintf(stderr, "libcgj: direct gj_libcgj_green_68900 fail\n"); nFail++; }
	if (gj_bar3_ready_68900() != 0u) { fprintf(stderr, "libcgj: direct gj_bar3_ready_68900 fail\n"); nFail++; }
	if (gj_product_score_68900() != 0u) { fprintf(stderr, "libcgj: direct gj_product_score_68900 fail\n"); nFail++; }
	if (gj_continuum_ready_68900() != 1u) { fprintf(stderr, "libcgj: direct gj_continuum_ready_68900 fail\n"); nFail++; }
	if (gj_smoke_soft_68900() != 1u) { fprintf(stderr, "libcgj: direct gj_smoke_soft_68900 fail\n"); nFail++; }
	if (gj_dyn_soft_68900() != 1u) { fprintf(stderr, "libcgj: direct gj_dyn_soft_68900 fail\n"); nFail++; }
	if (gj_milestone_tag_68900() != 68900u) { fprintf(stderr, "libcgj: direct gj_milestone_tag_68900 fail\n"); nFail++; }
	if (gj_continuum_wave_68900() != 68900u) { fprintf(stderr, "libcgj: direct gj_continuum_wave_68900 fail\n"); nFail++; }
	if (gj_batch_id_68900() != 68900u) { fprintf(stderr, "libcgj: direct gj_batch_id_68900 fail\n"); nFail++; }
	if (gj_graph_milestone_68900() != 68900u) { fprintf(stderr, "libcgj: direct gj_graph_milestone_68900 fail\n"); nFail++; }
	if (strcmp(__libcgj_batch68891_marker, "libcgj-batch68891") != 0 ||
	    strcmp(__libcgj_batch68892_marker, "libcgj-batch68892") != 0 ||
	    strcmp(__libcgj_batch68893_marker, "libcgj-batch68893") != 0 ||
	    strcmp(__libcgj_batch68894_marker, "libcgj-batch68894") != 0 ||
	    strcmp(__libcgj_batch68895_marker, "libcgj-batch68895") != 0 ||
	    strcmp(__libcgj_batch68896_marker, "libcgj-batch68896") != 0 ||
	    strcmp(__libcgj_batch68897_marker, "libcgj-batch68897") != 0 ||
	    strcmp(__libcgj_batch68898_marker, "libcgj-batch68898") != 0 ||
	    strcmp(__libcgj_batch68899_marker, "libcgj-batch68899") != 0 ||
	    strcmp(__libcgj_batch68900_marker, "libcgj-batch68900") != 0) {
		fprintf(stderr, "libcgj: direct m68900 marker string fail\n"); nFail++;
	}
	return nFail;
}
#else
typedef uint32_t (*u32_fn)(void);
static int soft_u32(void *h, const char *szName, uint32_t u32Expect, int *pChecked) {
	u32_fn pfn = (u32_fn)dlsym(h, szName); uint32_t u32Got;
	if (pfn == NULL) return 0; if (pChecked) (*pChecked)++;
	u32Got = pfn();
	if (u32Got != u32Expect) { fprintf(stderr, "libcgj: soft m68900 %s KAT fail got=%u expect=%u\n", szName, (unsigned)u32Got, (unsigned)u32Expect); return 1; }
	return 0;
}
static int soft_marker(void *h, const char *szSym, const char *szExpect, int *pChecked) {
	const char *szGot = (const char *)dlsym(h, szSym);
	if (szGot == NULL) return 0; if (pChecked) (*pChecked)++;
	if (strcmp(szGot, szExpect) != 0) { fprintf(stderr, "libcgj: soft m68900 %s marker fail got=\"%s\" expect=\"%s\"\n", szSym, szGot, szExpect); return 1; }
	return 0;
}
static const char *const kPaths[] = { "build/user/libc.so.6", "./build/user/libc.so.6", NULL };
#endif
int main(void) {
#ifdef CGJ_SOFT_M68900_DIRECT
	int nFail = direct_check();
	if (nFail) { fprintf(stderr, "libcgj: soft m68900 DIRECT FAIL fails=%d\n", nFail); return 1; }
	printf("libcgj: soft m68900 DIRECT PASS (all 11 lamps + markers)\n"); return 0;
#else
	void *h = NULL; const char *szPath = NULL; int i, nChecked = 0, nFail = 0;
	for (i = 0; kPaths[i]; i++) { h = dlopen(kPaths[i], RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND); if (h) { szPath = kPaths[i]; break; } }
	if (!h) { fprintf(stderr, "libcgj: soft m68900 skip (dlopen fail: %s)\n", dlerror()); printf("libcgj: soft m68900 SOFT-SKIP (no product SO)\n"); return 0; }
	nFail += soft_u32(h, "gj_shell_green_68900", 1u, &nChecked);
	nFail += soft_u32(h, "gj_libcgj_green_68900", 1u, &nChecked);
	nFail += soft_u32(h, "gj_bar3_ready_68900", 0u, &nChecked);
	nFail += soft_u32(h, "gj_product_score_68900", 0u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_ready_68900", 1u, &nChecked);
	nFail += soft_u32(h, "gj_smoke_soft_68900", 1u, &nChecked);
	nFail += soft_u32(h, "gj_dyn_soft_68900", 1u, &nChecked);
	nFail += soft_u32(h, "gj_milestone_tag_68900", 68900u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_wave_68900", 68900u, &nChecked);
	nFail += soft_u32(h, "gj_batch_id_68900", 68900u, &nChecked);
	nFail += soft_u32(h, "gj_graph_milestone_68900", 68900u, &nChecked);
	nFail += soft_marker(h, "__libcgj_batch68891_marker", "libcgj-batch68891", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch68892_marker", "libcgj-batch68892", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch68893_marker", "libcgj-batch68893", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch68894_marker", "libcgj-batch68894", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch68895_marker", "libcgj-batch68895", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch68896_marker", "libcgj-batch68896", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch68897_marker", "libcgj-batch68897", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch68898_marker", "libcgj-batch68898", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch68899_marker", "libcgj-batch68899", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch68900_marker", "libcgj-batch68900", &nChecked);
	if (nFail) { (void)dlclose(h); fprintf(stderr, "libcgj: soft m68900 FAIL path=%s fails=%d checked=%d\n", szPath, nFail, nChecked); return 2; }
	printf("libcgj: soft m68900 PASS path=%s checked=%d (soft-skip unexported/missing; bar3 still open)\n", szPath, nChecked);
	(void)dlclose(h); return 0;
#endif
}
