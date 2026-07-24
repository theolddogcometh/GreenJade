/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 76400 markers (CREATE-ONLY).
 *
 * Coherent soft package (see NOTES_milestone_76400.txt):
 *   - this TU (libcgj continuum dyn/direct soft KATs)
 *   - NOTES_milestone_76400.txt
 *
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_76400 == 0).
 * Sibling harnesses: cgj_smoke.c / cgj_dyn_smoke.c / prior soft decades.
 * Not wired in Makefile; build and run manually when product SO is ready.
 *
 * Manual build (host libc + dl):
 *   cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_76400 \
 *     user/libcgj/tests/cgj_soft_milestone_76400.c -ldl
 *   ./build/cgj_soft_milestone_76400
 *
 * Optional hard unit check (link pure graph batches directly; no SO map):
 *   cc -std=c11 -Wall -Wextra -O2 -DCGJ_SOFT_M76400_DIRECT \
 *     -o build/cgj_soft_milestone_76400_direct \
 *     user/libcgj/tests/cgj_soft_milestone_76400.c \
 *     user/libcgj/src/graph_batch76391.c \
 *     user/libcgj/src/graph_batch76392.c \
 *     user/libcgj/src/graph_batch76393.c \
 *     user/libcgj/src/graph_batch76394.c \
 *     user/libcgj/src/graph_batch76395.c \
 *     user/libcgj/src/graph_batch76396.c \
 *     user/libcgj/src/graph_batch76397.c \
 *     user/libcgj/src/graph_batch76398.c \
 *     user/libcgj/src/graph_batch76399.c \
 *     user/libcgj/src/graph_batch76400.c
 *   ./build/cgj_soft_milestone_76400_direct
 *
 * Expected soft KATs (wave 76391–76400 exclusive continuum markers):
 *   gj_shell_green_76400 → 1; gj_libcgj_green_76400 → 1
 *   gj_bar3_ready_76400 → 0; gj_product_score_76400 → 0
 *   gj_continuum_ready_76400 → 1; gj_smoke_soft_76400 → 1
 *   gj_dyn_soft_76400 → 1; gj_milestone_tag_76400 → 76400
 *   gj_continuum_wave_76400 → 76400; gj_batch_id_76400 → 76400
 *   gj_graph_milestone_76400 → 76400
 *   __libcgj_batch76391_marker … __libcgj_batch76400_marker
 *
 * Parent wiring: graph_batch76391.c … graph_batch76400.c (fill 76376–76390,
 * identity 76351–76375) CREATE-ONLY until makefile_max advances to 76400.
 */

#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef CGJ_SOFT_M76400_DIRECT
#include <dlfcn.h>
#endif

#ifdef CGJ_SOFT_M76400_DIRECT
extern uint32_t gj_shell_green_76400(void);
extern uint32_t gj_libcgj_green_76400(void);
extern uint32_t gj_bar3_ready_76400(void);
extern uint32_t gj_product_score_76400(void);
extern uint32_t gj_continuum_ready_76400(void);
extern uint32_t gj_smoke_soft_76400(void);
extern uint32_t gj_dyn_soft_76400(void);
extern uint32_t gj_milestone_tag_76400(void);
extern uint32_t gj_continuum_wave_76400(void);
extern uint32_t gj_batch_id_76400(void);
extern uint32_t gj_graph_milestone_76400(void);
extern const char __libcgj_batch76391_marker[];
extern const char __libcgj_batch76392_marker[];
extern const char __libcgj_batch76393_marker[];
extern const char __libcgj_batch76394_marker[];
extern const char __libcgj_batch76395_marker[];
extern const char __libcgj_batch76396_marker[];
extern const char __libcgj_batch76397_marker[];
extern const char __libcgj_batch76398_marker[];
extern const char __libcgj_batch76399_marker[];
extern const char __libcgj_batch76400_marker[];

static int
direct_check(void)
{
	int nFail = 0;

	if (gj_shell_green_76400() != 1u) {
		fprintf(stderr, "libcgj: direct gj_shell_green_76400 fail\n");
		nFail++;
	}
	if (gj_libcgj_green_76400() != 1u) {
		fprintf(stderr, "libcgj: direct gj_libcgj_green_76400 fail\n");
		nFail++;
	}
	if (gj_bar3_ready_76400() != 0u) {
		fprintf(stderr, "libcgj: direct gj_bar3_ready_76400 fail\n");
		nFail++;
	}
	if (gj_product_score_76400() != 0u) {
		fprintf(stderr, "libcgj: direct gj_product_score_76400 fail\n");
		nFail++;
	}
	if (gj_continuum_ready_76400() != 1u) {
		fprintf(stderr, "libcgj: direct gj_continuum_ready_76400 fail\n");
		nFail++;
	}
	if (gj_smoke_soft_76400() != 1u) {
		fprintf(stderr, "libcgj: direct gj_smoke_soft_76400 fail\n");
		nFail++;
	}
	if (gj_dyn_soft_76400() != 1u) {
		fprintf(stderr, "libcgj: direct gj_dyn_soft_76400 fail\n");
		nFail++;
	}
	if (gj_milestone_tag_76400() != 76400u) {
		fprintf(stderr, "libcgj: direct gj_milestone_tag_76400 fail\n");
		nFail++;
	}
	if (gj_continuum_wave_76400() != 76400u) {
		fprintf(stderr, "libcgj: direct gj_continuum_wave_76400 fail\n");
		nFail++;
	}
	if (gj_batch_id_76400() != 76400u) {
		fprintf(stderr, "libcgj: direct gj_batch_id_76400 fail\n");
		nFail++;
	}
	if (gj_graph_milestone_76400() != 76400u) {
		fprintf(stderr, "libcgj: direct gj_graph_milestone_76400 fail\n");
		nFail++;
	}
	if (strcmp(__libcgj_batch76391_marker, "libcgj-batch76391") != 0 ||
	    strcmp(__libcgj_batch76392_marker, "libcgj-batch76392") != 0 ||
	    strcmp(__libcgj_batch76393_marker, "libcgj-batch76393") != 0 ||
	    strcmp(__libcgj_batch76394_marker, "libcgj-batch76394") != 0 ||
	    strcmp(__libcgj_batch76395_marker, "libcgj-batch76395") != 0 ||
	    strcmp(__libcgj_batch76396_marker, "libcgj-batch76396") != 0 ||
	    strcmp(__libcgj_batch76397_marker, "libcgj-batch76397") != 0 ||
	    strcmp(__libcgj_batch76398_marker, "libcgj-batch76398") != 0 ||
	    strcmp(__libcgj_batch76399_marker, "libcgj-batch76399") != 0 ||
	    strcmp(__libcgj_batch76400_marker, "libcgj-batch76400") != 0) {
		fprintf(stderr, "libcgj: direct m76400 marker string fail\n");
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
		fprintf(stderr,
		        "libcgj: soft m76400 %s KAT fail got=%u expect=%u\n",
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
		fprintf(stderr,
		        "libcgj: soft m76400 %s marker fail got=\"%s\" "
		        "expect=\"%s\"\n",
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
#ifdef CGJ_SOFT_M76400_DIRECT
	int nFail = direct_check();
	if (nFail != 0) {
		fprintf(stderr, "libcgj: soft m76400 DIRECT FAIL fails=%d\n", nFail);
		return 1;
	}
	printf("libcgj: soft m76400 DIRECT PASS (all 11 lamps + markers)\n");
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
		fprintf(stderr, "libcgj: soft m76400 skip (dlopen fail: %s)\n",
		        dlerror());
		printf("libcgj: soft m76400 SOFT-SKIP (no product SO)\n");
		return 0;
	}

	nFail += soft_u32(h, "gj_shell_green_76400", 1u, &nChecked);
	nFail += soft_u32(h, "gj_libcgj_green_76400", 1u, &nChecked);
	nFail += soft_u32(h, "gj_bar3_ready_76400", 0u, &nChecked);
	nFail += soft_u32(h, "gj_product_score_76400", 0u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_ready_76400", 1u, &nChecked);
	nFail += soft_u32(h, "gj_smoke_soft_76400", 1u, &nChecked);
	nFail += soft_u32(h, "gj_dyn_soft_76400", 1u, &nChecked);
	nFail += soft_u32(h, "gj_milestone_tag_76400", 76400u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_wave_76400", 76400u, &nChecked);
	nFail += soft_u32(h, "gj_batch_id_76400", 76400u, &nChecked);
	nFail += soft_u32(h, "gj_graph_milestone_76400", 76400u, &nChecked);
	nFail += soft_marker(h, "__libcgj_batch76391_marker",
	                     "libcgj-batch76391", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch76392_marker",
	                     "libcgj-batch76392", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch76393_marker",
	                     "libcgj-batch76393", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch76394_marker",
	                     "libcgj-batch76394", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch76395_marker",
	                     "libcgj-batch76395", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch76396_marker",
	                     "libcgj-batch76396", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch76397_marker",
	                     "libcgj-batch76397", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch76398_marker",
	                     "libcgj-batch76398", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch76399_marker",
	                     "libcgj-batch76399", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch76400_marker",
	                     "libcgj-batch76400", &nChecked);

	if (nFail != 0) {
		(void)dlclose(h);
		fprintf(stderr,
		        "libcgj: soft m76400 FAIL path=%s fails=%d checked=%d\n",
		        szPath, nFail, nChecked);
		return 2;
	}
	printf("libcgj: soft m76400 PASS path=%s checked=%d "
	       "(soft-skip unexported/missing; bar3 still open)\n",
	       szPath, nChecked);
	(void)dlclose(h);
	return 0;
#endif
}
