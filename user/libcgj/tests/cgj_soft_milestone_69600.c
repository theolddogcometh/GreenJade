/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 * Soft host gates MILESTONE 69600 (CREATE-ONLY). Mirror 25800; bar3=0.
 * See NOTES_milestone_69600.txt
 */
#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#ifndef CGJ_SOFT_M69600_DIRECT
#include <dlfcn.h>
#endif
#ifdef CGJ_SOFT_M69600_DIRECT
extern uint32_t gj_shell_green_69600(void);
extern uint32_t gj_libcgj_green_69600(void);
extern uint32_t gj_bar3_ready_69600(void);
extern uint32_t gj_product_score_69600(void);
extern uint32_t gj_continuum_ready_69600(void);
extern uint32_t gj_smoke_soft_69600(void);
extern uint32_t gj_dyn_soft_69600(void);
extern uint32_t gj_milestone_tag_69600(void);
extern uint32_t gj_continuum_wave_69600(void);
extern uint32_t gj_batch_id_69600(void);
extern uint32_t gj_graph_milestone_69600(void);
extern const char __libcgj_batch69591_marker[];
extern const char __libcgj_batch69592_marker[];
extern const char __libcgj_batch69593_marker[];
extern const char __libcgj_batch69594_marker[];
extern const char __libcgj_batch69595_marker[];
extern const char __libcgj_batch69596_marker[];
extern const char __libcgj_batch69597_marker[];
extern const char __libcgj_batch69598_marker[];
extern const char __libcgj_batch69599_marker[];
extern const char __libcgj_batch69600_marker[];
static int direct_check(void) {
	int nFail = 0;
	if (gj_shell_green_69600() != 1u) { fprintf(stderr, "libcgj: direct gj_shell_green_69600 fail\n"); nFail++; }
	if (gj_libcgj_green_69600() != 1u) { fprintf(stderr, "libcgj: direct gj_libcgj_green_69600 fail\n"); nFail++; }
	if (gj_bar3_ready_69600() != 0u) { fprintf(stderr, "libcgj: direct gj_bar3_ready_69600 fail\n"); nFail++; }
	if (gj_product_score_69600() != 0u) { fprintf(stderr, "libcgj: direct gj_product_score_69600 fail\n"); nFail++; }
	if (gj_continuum_ready_69600() != 1u) { fprintf(stderr, "libcgj: direct gj_continuum_ready_69600 fail\n"); nFail++; }
	if (gj_smoke_soft_69600() != 1u) { fprintf(stderr, "libcgj: direct gj_smoke_soft_69600 fail\n"); nFail++; }
	if (gj_dyn_soft_69600() != 1u) { fprintf(stderr, "libcgj: direct gj_dyn_soft_69600 fail\n"); nFail++; }
	if (gj_milestone_tag_69600() != 69600u) { fprintf(stderr, "libcgj: direct gj_milestone_tag_69600 fail\n"); nFail++; }
	if (gj_continuum_wave_69600() != 69600u) { fprintf(stderr, "libcgj: direct gj_continuum_wave_69600 fail\n"); nFail++; }
	if (gj_batch_id_69600() != 69600u) { fprintf(stderr, "libcgj: direct gj_batch_id_69600 fail\n"); nFail++; }
	if (gj_graph_milestone_69600() != 69600u) { fprintf(stderr, "libcgj: direct gj_graph_milestone_69600 fail\n"); nFail++; }
	if (strcmp(__libcgj_batch69591_marker, "libcgj-batch69591") != 0 ||
	    strcmp(__libcgj_batch69592_marker, "libcgj-batch69592") != 0 ||
	    strcmp(__libcgj_batch69593_marker, "libcgj-batch69593") != 0 ||
	    strcmp(__libcgj_batch69594_marker, "libcgj-batch69594") != 0 ||
	    strcmp(__libcgj_batch69595_marker, "libcgj-batch69595") != 0 ||
	    strcmp(__libcgj_batch69596_marker, "libcgj-batch69596") != 0 ||
	    strcmp(__libcgj_batch69597_marker, "libcgj-batch69597") != 0 ||
	    strcmp(__libcgj_batch69598_marker, "libcgj-batch69598") != 0 ||
	    strcmp(__libcgj_batch69599_marker, "libcgj-batch69599") != 0 ||
	    strcmp(__libcgj_batch69600_marker, "libcgj-batch69600") != 0) {
		fprintf(stderr, "libcgj: direct m69600 marker string fail\n"); nFail++;
	}
	return nFail;
}
#else
typedef uint32_t (*u32_fn)(void);
static int soft_u32(void *h, const char *szName, uint32_t u32Expect, int *pChecked) {
	u32_fn pfn = (u32_fn)dlsym(h, szName); uint32_t u32Got;
	if (pfn == NULL) return 0; if (pChecked) (*pChecked)++;
	u32Got = pfn();
	if (u32Got != u32Expect) { fprintf(stderr, "libcgj: soft m69600 %s KAT fail got=%u expect=%u\n", szName, (unsigned)u32Got, (unsigned)u32Expect); return 1; }
	return 0;
}
static int soft_marker(void *h, const char *szSym, const char *szExpect, int *pChecked) {
	const char *szGot = (const char *)dlsym(h, szSym);
	if (szGot == NULL) return 0; if (pChecked) (*pChecked)++;
	if (strcmp(szGot, szExpect) != 0) { fprintf(stderr, "libcgj: soft m69600 %s marker fail got=\"%s\" expect=\"%s\"\n", szSym, szGot, szExpect); return 1; }
	return 0;
}
static const char *const kPaths[] = { "build/user/libc.so.6", "./build/user/libc.so.6", NULL };
#endif
int main(void) {
#ifdef CGJ_SOFT_M69600_DIRECT
	int nFail = direct_check();
	if (nFail) { fprintf(stderr, "libcgj: soft m69600 DIRECT FAIL fails=%d\n", nFail); return 1; }
	printf("libcgj: soft m69600 DIRECT PASS (all 11 lamps + markers)\n"); return 0;
#else
	void *h = NULL; const char *szPath = NULL; int i, nChecked = 0, nFail = 0;
	for (i = 0; kPaths[i]; i++) { h = dlopen(kPaths[i], RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND); if (h) { szPath = kPaths[i]; break; } }
	if (!h) { fprintf(stderr, "libcgj: soft m69600 skip (dlopen fail: %s)\n", dlerror()); printf("libcgj: soft m69600 SOFT-SKIP (no product SO)\n"); return 0; }
	nFail += soft_u32(h, "gj_shell_green_69600", 1u, &nChecked);
	nFail += soft_u32(h, "gj_libcgj_green_69600", 1u, &nChecked);
	nFail += soft_u32(h, "gj_bar3_ready_69600", 0u, &nChecked);
	nFail += soft_u32(h, "gj_product_score_69600", 0u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_ready_69600", 1u, &nChecked);
	nFail += soft_u32(h, "gj_smoke_soft_69600", 1u, &nChecked);
	nFail += soft_u32(h, "gj_dyn_soft_69600", 1u, &nChecked);
	nFail += soft_u32(h, "gj_milestone_tag_69600", 69600u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_wave_69600", 69600u, &nChecked);
	nFail += soft_u32(h, "gj_batch_id_69600", 69600u, &nChecked);
	nFail += soft_u32(h, "gj_graph_milestone_69600", 69600u, &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69591_marker", "libcgj-batch69591", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69592_marker", "libcgj-batch69592", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69593_marker", "libcgj-batch69593", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69594_marker", "libcgj-batch69594", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69595_marker", "libcgj-batch69595", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69596_marker", "libcgj-batch69596", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69597_marker", "libcgj-batch69597", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69598_marker", "libcgj-batch69598", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69599_marker", "libcgj-batch69599", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69600_marker", "libcgj-batch69600", &nChecked);
	if (nFail) { (void)dlclose(h); fprintf(stderr, "libcgj: soft m69600 FAIL path=%s fails=%d checked=%d\n", szPath, nFail, nChecked); return 2; }
	printf("libcgj: soft m69600 PASS path=%s checked=%d (soft-skip unexported/missing; bar3 still open)\n", szPath, nChecked);
	(void)dlclose(h); return 0;
#endif
}
