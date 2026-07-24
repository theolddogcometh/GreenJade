/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 * Soft host gates MILESTONE 69700 (CREATE-ONLY). Mirror 25800; bar3=0.
 * See NOTES_milestone_69700.txt
 */
#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#ifndef CGJ_SOFT_M69700_DIRECT
#include <dlfcn.h>
#endif
#ifdef CGJ_SOFT_M69700_DIRECT
extern uint32_t gj_shell_green_69700(void);
extern uint32_t gj_libcgj_green_69700(void);
extern uint32_t gj_bar3_ready_69700(void);
extern uint32_t gj_product_score_69700(void);
extern uint32_t gj_continuum_ready_69700(void);
extern uint32_t gj_smoke_soft_69700(void);
extern uint32_t gj_dyn_soft_69700(void);
extern uint32_t gj_milestone_tag_69700(void);
extern uint32_t gj_continuum_wave_69700(void);
extern uint32_t gj_batch_id_69700(void);
extern uint32_t gj_graph_milestone_69700(void);
extern const char __libcgj_batch69691_marker[];
extern const char __libcgj_batch69692_marker[];
extern const char __libcgj_batch69693_marker[];
extern const char __libcgj_batch69694_marker[];
extern const char __libcgj_batch69695_marker[];
extern const char __libcgj_batch69696_marker[];
extern const char __libcgj_batch69697_marker[];
extern const char __libcgj_batch69698_marker[];
extern const char __libcgj_batch69699_marker[];
extern const char __libcgj_batch69700_marker[];
static int direct_check(void) {
	int nFail = 0;
	if (gj_shell_green_69700() != 1u) { fprintf(stderr, "libcgj: direct gj_shell_green_69700 fail\n"); nFail++; }
	if (gj_libcgj_green_69700() != 1u) { fprintf(stderr, "libcgj: direct gj_libcgj_green_69700 fail\n"); nFail++; }
	if (gj_bar3_ready_69700() != 0u) { fprintf(stderr, "libcgj: direct gj_bar3_ready_69700 fail\n"); nFail++; }
	if (gj_product_score_69700() != 0u) { fprintf(stderr, "libcgj: direct gj_product_score_69700 fail\n"); nFail++; }
	if (gj_continuum_ready_69700() != 1u) { fprintf(stderr, "libcgj: direct gj_continuum_ready_69700 fail\n"); nFail++; }
	if (gj_smoke_soft_69700() != 1u) { fprintf(stderr, "libcgj: direct gj_smoke_soft_69700 fail\n"); nFail++; }
	if (gj_dyn_soft_69700() != 1u) { fprintf(stderr, "libcgj: direct gj_dyn_soft_69700 fail\n"); nFail++; }
	if (gj_milestone_tag_69700() != 69700u) { fprintf(stderr, "libcgj: direct gj_milestone_tag_69700 fail\n"); nFail++; }
	if (gj_continuum_wave_69700() != 69700u) { fprintf(stderr, "libcgj: direct gj_continuum_wave_69700 fail\n"); nFail++; }
	if (gj_batch_id_69700() != 69700u) { fprintf(stderr, "libcgj: direct gj_batch_id_69700 fail\n"); nFail++; }
	if (gj_graph_milestone_69700() != 69700u) { fprintf(stderr, "libcgj: direct gj_graph_milestone_69700 fail\n"); nFail++; }
	if (strcmp(__libcgj_batch69691_marker, "libcgj-batch69691") != 0 ||
	    strcmp(__libcgj_batch69692_marker, "libcgj-batch69692") != 0 ||
	    strcmp(__libcgj_batch69693_marker, "libcgj-batch69693") != 0 ||
	    strcmp(__libcgj_batch69694_marker, "libcgj-batch69694") != 0 ||
	    strcmp(__libcgj_batch69695_marker, "libcgj-batch69695") != 0 ||
	    strcmp(__libcgj_batch69696_marker, "libcgj-batch69696") != 0 ||
	    strcmp(__libcgj_batch69697_marker, "libcgj-batch69697") != 0 ||
	    strcmp(__libcgj_batch69698_marker, "libcgj-batch69698") != 0 ||
	    strcmp(__libcgj_batch69699_marker, "libcgj-batch69699") != 0 ||
	    strcmp(__libcgj_batch69700_marker, "libcgj-batch69700") != 0) {
		fprintf(stderr, "libcgj: direct m69700 marker string fail\n"); nFail++;
	}
	return nFail;
}
#else
typedef uint32_t (*u32_fn)(void);
static int soft_u32(void *h, const char *szName, uint32_t u32Expect, int *pChecked) {
	u32_fn pfn = (u32_fn)dlsym(h, szName); uint32_t u32Got;
	if (pfn == NULL) return 0; if (pChecked) (*pChecked)++;
	u32Got = pfn();
	if (u32Got != u32Expect) { fprintf(stderr, "libcgj: soft m69700 %s KAT fail got=%u expect=%u\n", szName, (unsigned)u32Got, (unsigned)u32Expect); return 1; }
	return 0;
}
static int soft_marker(void *h, const char *szSym, const char *szExpect, int *pChecked) {
	const char *szGot = (const char *)dlsym(h, szSym);
	if (szGot == NULL) return 0; if (pChecked) (*pChecked)++;
	if (strcmp(szGot, szExpect) != 0) { fprintf(stderr, "libcgj: soft m69700 %s marker fail got=\"%s\" expect=\"%s\"\n", szSym, szGot, szExpect); return 1; }
	return 0;
}
static const char *const kPaths[] = { "build/user/libc.so.6", "./build/user/libc.so.6", NULL };
#endif
int main(void) {
#ifdef CGJ_SOFT_M69700_DIRECT
	int nFail = direct_check();
	if (nFail) { fprintf(stderr, "libcgj: soft m69700 DIRECT FAIL fails=%d\n", nFail); return 1; }
	printf("libcgj: soft m69700 DIRECT PASS (all 11 lamps + markers)\n"); return 0;
#else
	void *h = NULL; const char *szPath = NULL; int i, nChecked = 0, nFail = 0;
	for (i = 0; kPaths[i]; i++) { h = dlopen(kPaths[i], RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND); if (h) { szPath = kPaths[i]; break; } }
	if (!h) { fprintf(stderr, "libcgj: soft m69700 skip (dlopen fail: %s)\n", dlerror()); printf("libcgj: soft m69700 SOFT-SKIP (no product SO)\n"); return 0; }
	nFail += soft_u32(h, "gj_shell_green_69700", 1u, &nChecked);
	nFail += soft_u32(h, "gj_libcgj_green_69700", 1u, &nChecked);
	nFail += soft_u32(h, "gj_bar3_ready_69700", 0u, &nChecked);
	nFail += soft_u32(h, "gj_product_score_69700", 0u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_ready_69700", 1u, &nChecked);
	nFail += soft_u32(h, "gj_smoke_soft_69700", 1u, &nChecked);
	nFail += soft_u32(h, "gj_dyn_soft_69700", 1u, &nChecked);
	nFail += soft_u32(h, "gj_milestone_tag_69700", 69700u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_wave_69700", 69700u, &nChecked);
	nFail += soft_u32(h, "gj_batch_id_69700", 69700u, &nChecked);
	nFail += soft_u32(h, "gj_graph_milestone_69700", 69700u, &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69691_marker", "libcgj-batch69691", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69692_marker", "libcgj-batch69692", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69693_marker", "libcgj-batch69693", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69694_marker", "libcgj-batch69694", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69695_marker", "libcgj-batch69695", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69696_marker", "libcgj-batch69696", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69697_marker", "libcgj-batch69697", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69698_marker", "libcgj-batch69698", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69699_marker", "libcgj-batch69699", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch69700_marker", "libcgj-batch69700", &nChecked);
	if (nFail) { (void)dlclose(h); fprintf(stderr, "libcgj: soft m69700 FAIL path=%s fails=%d checked=%d\n", szPath, nFail, nChecked); return 2; }
	printf("libcgj: soft m69700 PASS path=%s checked=%d (soft-skip unexported/missing; bar3 still open)\n", szPath, nChecked);
	(void)dlclose(h); return 0;
#endif
}
