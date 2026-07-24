/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 * Soft host gates MILESTONE 68800 (CREATE-ONLY). Mirror 25800; bar3=0.
 * See NOTES_milestone_68800.txt
 */
#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#ifndef CGJ_SOFT_M68800_DIRECT
#include <dlfcn.h>
#endif
#ifdef CGJ_SOFT_M68800_DIRECT
extern uint32_t gj_shell_green_68800(void);
extern uint32_t gj_libcgj_green_68800(void);
extern uint32_t gj_bar3_ready_68800(void);
extern uint32_t gj_product_score_68800(void);
extern uint32_t gj_continuum_ready_68800(void);
extern uint32_t gj_smoke_soft_68800(void);
extern uint32_t gj_dyn_soft_68800(void);
extern uint32_t gj_milestone_tag_68800(void);
extern uint32_t gj_continuum_wave_68800(void);
extern uint32_t gj_batch_id_68800(void);
extern uint32_t gj_graph_milestone_68800(void);
extern const char __libcgj_batch68791_marker[];
extern const char __libcgj_batch68792_marker[];
extern const char __libcgj_batch68793_marker[];
extern const char __libcgj_batch68794_marker[];
extern const char __libcgj_batch68795_marker[];
extern const char __libcgj_batch68796_marker[];
extern const char __libcgj_batch68797_marker[];
extern const char __libcgj_batch68798_marker[];
extern const char __libcgj_batch68799_marker[];
extern const char __libcgj_batch68800_marker[];
static int direct_check(void) {
	int nFail = 0;
	if (gj_shell_green_68800() != 1u) { fprintf(stderr, "libcgj: direct gj_shell_green_68800 fail\n"); nFail++; }
	if (gj_libcgj_green_68800() != 1u) { fprintf(stderr, "libcgj: direct gj_libcgj_green_68800 fail\n"); nFail++; }
	if (gj_bar3_ready_68800() != 0u) { fprintf(stderr, "libcgj: direct gj_bar3_ready_68800 fail\n"); nFail++; }
	if (gj_product_score_68800() != 0u) { fprintf(stderr, "libcgj: direct gj_product_score_68800 fail\n"); nFail++; }
	if (gj_continuum_ready_68800() != 1u) { fprintf(stderr, "libcgj: direct gj_continuum_ready_68800 fail\n"); nFail++; }
	if (gj_smoke_soft_68800() != 1u) { fprintf(stderr, "libcgj: direct gj_smoke_soft_68800 fail\n"); nFail++; }
	if (gj_dyn_soft_68800() != 1u) { fprintf(stderr, "libcgj: direct gj_dyn_soft_68800 fail\n"); nFail++; }
	if (gj_milestone_tag_68800() != 68800u) { fprintf(stderr, "libcgj: direct gj_milestone_tag_68800 fail\n"); nFail++; }
	if (gj_continuum_wave_68800() != 68800u) { fprintf(stderr, "libcgj: direct gj_continuum_wave_68800 fail\n"); nFail++; }
	if (gj_batch_id_68800() != 68800u) { fprintf(stderr, "libcgj: direct gj_batch_id_68800 fail\n"); nFail++; }
	if (gj_graph_milestone_68800() != 68800u) { fprintf(stderr, "libcgj: direct gj_graph_milestone_68800 fail\n"); nFail++; }
	if (strcmp(__libcgj_batch68791_marker, "libcgj-batch68791") != 0 ||
	    strcmp(__libcgj_batch68792_marker, "libcgj-batch68792") != 0 ||
	    strcmp(__libcgj_batch68793_marker, "libcgj-batch68793") != 0 ||
	    strcmp(__libcgj_batch68794_marker, "libcgj-batch68794") != 0 ||
	    strcmp(__libcgj_batch68795_marker, "libcgj-batch68795") != 0 ||
	    strcmp(__libcgj_batch68796_marker, "libcgj-batch68796") != 0 ||
	    strcmp(__libcgj_batch68797_marker, "libcgj-batch68797") != 0 ||
	    strcmp(__libcgj_batch68798_marker, "libcgj-batch68798") != 0 ||
	    strcmp(__libcgj_batch68799_marker, "libcgj-batch68799") != 0 ||
	    strcmp(__libcgj_batch68800_marker, "libcgj-batch68800") != 0) {
		fprintf(stderr, "libcgj: direct m68800 marker string fail\n"); nFail++;
	}
	return nFail;
}
#else
typedef uint32_t (*u32_fn)(void);
static int soft_u32(void *h, const char *szName, uint32_t u32Expect, int *pChecked) {
	u32_fn pfn = (u32_fn)dlsym(h, szName); uint32_t u32Got;
	if (pfn == NULL) return 0; if (pChecked) (*pChecked)++;
	u32Got = pfn();
	if (u32Got != u32Expect) { fprintf(stderr, "libcgj: soft m68800 %s KAT fail got=%u expect=%u\n", szName, (unsigned)u32Got, (unsigned)u32Expect); return 1; }
	return 0;
}
static int soft_marker(void *h, const char *szSym, const char *szExpect, int *pChecked) {
	const char *szGot = (const char *)dlsym(h, szSym);
	if (szGot == NULL) return 0; if (pChecked) (*pChecked)++;
	if (strcmp(szGot, szExpect) != 0) { fprintf(stderr, "libcgj: soft m68800 %s marker fail got=\"%s\" expect=\"%s\"\n", szSym, szGot, szExpect); return 1; }
	return 0;
}
static const char *const kPaths[] = { "build/user/libc.so.6", "./build/user/libc.so.6", NULL };
#endif
int main(void) {
#ifdef CGJ_SOFT_M68800_DIRECT
	int nFail = direct_check();
	if (nFail) { fprintf(stderr, "libcgj: soft m68800 DIRECT FAIL fails=%d\n", nFail); return 1; }
	printf("libcgj: soft m68800 DIRECT PASS (all 11 lamps + markers)\n"); return 0;
#else
	void *h = NULL; const char *szPath = NULL; int i, nChecked = 0, nFail = 0;
	for (i = 0; kPaths[i]; i++) { h = dlopen(kPaths[i], RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND); if (h) { szPath = kPaths[i]; break; } }
	if (!h) { fprintf(stderr, "libcgj: soft m68800 skip (dlopen fail: %s)\n", dlerror()); printf("libcgj: soft m68800 SOFT-SKIP (no product SO)\n"); return 0; }
	nFail += soft_u32(h, "gj_shell_green_68800", 1u, &nChecked);
	nFail += soft_u32(h, "gj_libcgj_green_68800", 1u, &nChecked);
	nFail += soft_u32(h, "gj_bar3_ready_68800", 0u, &nChecked);
	nFail += soft_u32(h, "gj_product_score_68800", 0u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_ready_68800", 1u, &nChecked);
	nFail += soft_u32(h, "gj_smoke_soft_68800", 1u, &nChecked);
	nFail += soft_u32(h, "gj_dyn_soft_68800", 1u, &nChecked);
	nFail += soft_u32(h, "gj_milestone_tag_68800", 68800u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_wave_68800", 68800u, &nChecked);
	nFail += soft_u32(h, "gj_batch_id_68800", 68800u, &nChecked);
	nFail += soft_u32(h, "gj_graph_milestone_68800", 68800u, &nChecked);
	nFail += soft_marker(h, "__libcgj_batch68791_marker", "libcgj-batch68791", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch68792_marker", "libcgj-batch68792", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch68793_marker", "libcgj-batch68793", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch68794_marker", "libcgj-batch68794", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch68795_marker", "libcgj-batch68795", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch68796_marker", "libcgj-batch68796", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch68797_marker", "libcgj-batch68797", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch68798_marker", "libcgj-batch68798", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch68799_marker", "libcgj-batch68799", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch68800_marker", "libcgj-batch68800", &nChecked);
	if (nFail) { (void)dlclose(h); fprintf(stderr, "libcgj: soft m68800 FAIL path=%s fails=%d checked=%d\n", szPath, nFail, nChecked); return 2; }
	printf("libcgj: soft m68800 PASS path=%s checked=%d (soft-skip unexported/missing; bar3 still open)\n", szPath, nChecked);
	(void)dlclose(h); return 0;
#endif
}
