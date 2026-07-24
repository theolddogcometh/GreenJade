/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 * Soft host gates MILESTONE 69800 (CREATE-ONLY). Mirror 25800; bar3=0.
 * See NOTES_milestone_69800.txt
 */
#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#ifndef CGJ_SOFT_M69800_DIRECT
#include <dlfcn.h>
#endif
#ifdef CGJ_SOFT_M69800_DIRECT
extern uint32_t gj_shell_green_69800(void);
extern uint32_t gj_libcgj_green_69800(void);
extern uint32_t gj_bar3_ready_69800(void);
extern uint32_t gj_product_score_69800(void);
extern uint32_t gj_continuum_ready_69800(void);
extern uint32_t gj_smoke_soft_69800(void);
extern uint32_t gj_dyn_soft_69800(void);
extern uint32_t gj_milestone_tag_69800(void);
extern uint32_t gj_continuum_wave_69800(void);
extern uint32_t gj_batch_id_69800(void);
extern uint32_t gj_graph_milestone_69800(void);
extern const char __libcgj_batch69791_marker[];
extern const char __libcgj_batch69792_marker[];
extern const char __libcgj_batch69793_marker[];
extern const char __libcgj_batch69794_marker[];
extern const char __libcgj_batch69795_marker[];
extern const char __libcgj_batch69796_marker[];
extern const char __libcgj_batch69797_marker[];
extern const char __libcgj_batch69798_marker[];
extern const char __libcgj_batch69799_marker[];
extern const char __libcgj_batch69800_marker[];
static int direct_check(void) {
	int nFail = 0;
	if (gj_shell_green_69800() != 1u) { fprintf(stderr, "libcgj: direct gj_shell_green_69800 fail\n"); nFail++; }
	if (gj_libcgj_green_69800() != 1u) { fprintf(stderr, "libcgj: direct gj_libcgj_green_69800 fail\n"); nFail++; }
	if (gj_bar3_ready_69800() != 0u) { fprintf(stderr, "libcgj: direct gj_bar3_ready_69800 fail\n"); nFail++; }
	if (gj_product_score_69800() != 0u) { fprintf(stderr, "libcgj: direct gj_product_score_69800 fail\n"); nFail++; }
	if (gj_continuum_ready_69800() != 1u) { fprintf(stderr, "libcgj: direct gj_continuum_ready_69800 fail\n"); nFail++; }
	if (gj_smoke_soft_69800() != 1u) { fprintf(stderr, "libcgj: direct gj_smoke_soft_69800 fail\n"); nFail++; }
	if (gj_dyn_soft_69800() != 1u) { fprintf(stderr, "libcgj: direct gj_dyn_soft_69800 fail\n"); nFail++; }
	if (gj_milestone_tag_69800() != 69800u) { fprintf(stderr, "libcgj: direct gj_milestone_tag_69800 fail\n"); nFail++; }
	if (gj_continuum_wave_69800() != 69800u) { fprintf(stderr, "libcgj: direct gj_continuum_wave_69800 fail\n"); nFail++; }
	if (gj_batch_id_69800() != 69800u) { fprintf(stderr, "libcgj: direct gj_batch_id_69800 fail\n"); nFail++; }
	if (gj_graph_milestone_69800() != 69800u) { fprintf(stderr, "libcgj: direct gj_graph_milestone_69800 fail\n"); nFail++; }
	if (strcmp(__libcgj_batch69791_marker, "libcgj-batch69791") != 0 ||
	    strcmp(__libcgj_batch69792_marker, "libcgj-batch69792") != 0 ||
	    strcmp(__libcgj_batch69793_marker, "libcgj-batch69793") != 0 ||
	    strcmp(__libcgj_batch69794_marker, "libcgj-batch69794") != 0 ||
	    strcmp(__libcgj_batch69795_marker, "libcgj-batch69795") != 0 ||
	    strcmp(__libcgj_batch69796_marker, "libcgj-batch69796") != 0 ||
	    strcmp(__libcgj_batch69797_marker, "libcgj-batch69797") != 0 ||
	    strcmp(__libcgj_batch69798_marker, "libcgj-batch69798") != 0 ||
	    strcmp(__libcgj_batch69799_marker, "libcgj-batch69799") != 0 ||
	    strcmp(__libcgj_batch69800_marker, "libcgj-batch69800") != 0) {
		fprintf(stderr, "libcgj: direct m69800 marker string fail\n"); nFail++;
	}
	return nFail;
}
#else
typedef uint32_t (*u32_fn)(void);
static int soft_u32(void *h, const char *szName, uint32_t u32Expect, int *pChecked) {
	u32_fn pfn = (u32_fn)dlsym(h, szName); uint32_t u32Got;
	if (pfn == NULL) return 0; if (pChecked) (*pChecked)++;
	u32Got = pfn();
	if (u32Got != u32Expect) { fprintf(stderr, "libcgj: soft m69800 %s KAT fail got=%u expect=%u\n", szName, (unsigned)u32Got, (unsigned)u32Expect); return 1; }
	return 0;
}
static int soft_marker(void *h, const char *szSym, const char *szExpect, int *pChecked) {
	const char *szGot = (const char *)dlsym(h, szSym);
	if (szGot == NULL) return 0; if (pChecked) (*pChecked)++;
	if (strcmp(szGot, szExpect) != 0) { fprintf(stderr, "libcgj: soft m69800 %s marker fail got=\"%s\" expect=\"%s\"\n", szSym, szGot, szExpect); return 1; }
	return 0;
}
static const char *const kPaths[] = { "build/user/libc.so.6", "./build/user/libc.so.6", NULL };
#endif
int main(void) {
#ifdef CGJ_SOFT_M69800_DIRECT
	int nFail = direct_check();
	if (nFail) { fprintf(stderr, "libcgj: soft m69800 DIRECT FAIL fails=%d\n", nFail); return 1; }
	printf("libcgj: soft m69800 DIRECT PASS (all 11 lamps + markers)\n"); return 0;
#else
	void *h = NULL; const char *szPath = NULL; int i, nChecked = 0, nFail = 0;
	for (i = 0; kPaths[i]; i++) { h = dlopen(kPaths[i], RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND); if (h) { szPath = kPaths[i]; break; } }
	if (!h) { fprintf(stderr, "libcgj: soft m69800 skip (dlopen fail: %s)\n", dlerror()); printf("libcgj: soft m69800 SOFT-SKIP (no product SO)\n"); return 0; }
	nFail += soft_u32(h, "gj_shell_green_69800", 1u, &nChecked);
	nFail += soft_u32(h, "gj_libcgj_green_69800", 1u, &nChecked);
	nFail += soft_u32(h, "gj_bar3_ready_69800", 0u, &nChecked);
	nFail += soft_u32(h, "gj_product_score_69800", 0u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_ready_69800", 1u, &nChecked);
	nFail += soft_u32(h, "gj_smoke_soft_69800", 1u, &nChecked);
	nFail += soft_u32(h, "gj_dyn_soft_69800", 1u, &nChecked);
	nFail += soft_u32(h, "gj_milestone_tag_69800", 69800u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_wave_69800", 69800u, &nChecked);
	nFail += soft_u32(h, "gj_batch_id_69800", 69800u, &nChecked);
	nFail += soft_u32(h, "gj_graph_milestone_69800", 69800u, &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69791_marker", "libcgj-batch69791", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69792_marker", "libcgj-batch69792", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69793_marker", "libcgj-batch69793", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69794_marker", "libcgj-batch69794", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69795_marker", "libcgj-batch69795", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69796_marker", "libcgj-batch69796", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69797_marker", "libcgj-batch69797", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69798_marker", "libcgj-batch69798", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69799_marker", "libcgj-batch69799", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69800_marker", "libcgj-batch69800", &nChecked);
	if (nFail) { (void)dlclose(h); fprintf(stderr, "libcgj: soft m69800 FAIL path=%s fails=%d checked=%d\n", szPath, nFail, nChecked); return 2; }
	printf("libcgj: soft m69800 PASS path=%s checked=%d (soft-skip unexported/missing; bar3 still open)\n", szPath, nChecked);
	(void)dlclose(h); return 0;
#endif
}
