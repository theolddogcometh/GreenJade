/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 76300 markers (CREATE-ONLY).
 *
 * Coherent soft package (see NOTES_milestone_76300.txt):
 *   - this TU (libcgj continuum dyn/direct soft KATs)
 *   - NOTES_milestone_76300.txt
 *
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_76300 == 0).
 * Sibling harnesses: cgj_smoke.c / cgj_dyn_smoke.c / prior soft decades.
 * Not wired in Makefile; build and run manually when product SO is ready.
 *
 * Manual build (host libc + dl):
 *   cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_76300 \
 *     user/libcgj/tests/cgj_soft_milestone_76300.c -ldl
 *   ./build/cgj_soft_milestone_76300
 *
 * Optional hard unit check (link pure graph batches directly; no SO map):
 *   cc -std=c11 -Wall -Wextra -O2 -DCGJ_SOFT_M76300_DIRECT \
 *     -o build/cgj_soft_milestone_76300_direct \
 *     user/libcgj/tests/cgj_soft_milestone_76300.c \
 *     user/libcgj/src/graph_batch76291.c \
 *     user/libcgj/src/graph_batch76292.c \
 *     user/libcgj/src/graph_batch76293.c \
 *     user/libcgj/src/graph_batch76294.c \
 *     user/libcgj/src/graph_batch76295.c \
 *     user/libcgj/src/graph_batch76296.c \
 *     user/libcgj/src/graph_batch76297.c \
 *     user/libcgj/src/graph_batch76298.c \
 *     user/libcgj/src/graph_batch76299.c \
 *     user/libcgj/src/graph_batch76300.c
 *   ./build/cgj_soft_milestone_76300_direct
 *
 * Expected soft KATs (wave 76291–76300 exclusive continuum markers):
 *   gj_shell_green_76300      → 1
 *   gj_libcgj_green_76300     → 1
 *   gj_bar3_ready_76300       → 0   (bar3 still open)
 *   gj_product_score_76300    → 0   (product score still open)
 *   gj_continuum_ready_76300  → 1
 *   gj_smoke_soft_76300       → 1
 *   gj_dyn_soft_76300         → 1
 *   gj_milestone_tag_76300    → 76300
 *   gj_continuum_wave_76300   → 76300
 *   gj_batch_id_76300         → 76300
 *   gj_graph_milestone_76300  → 76300
 *   __libcgj_batch76291_marker … __libcgj_batch76300_marker
 *
 * Parent wiring: graph_batch76291.c … graph_batch76300.c (fill 76276–76290,
 * identity 76251–76275) CREATE-ONLY until makefile_max advances to 76300.
 */

#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef CGJ_SOFT_M76300_DIRECT
#include <dlfcn.h>
#endif

#ifdef CGJ_SOFT_M76300_DIRECT
extern uint32_t gj_shell_green_76300(void);
extern uint32_t gj_libcgj_green_76300(void);
extern uint32_t gj_bar3_ready_76300(void);
extern uint32_t gj_product_score_76300(void);
extern uint32_t gj_continuum_ready_76300(void);
extern uint32_t gj_smoke_soft_76300(void);
extern uint32_t gj_dyn_soft_76300(void);
extern uint32_t gj_milestone_tag_76300(void);
extern uint32_t gj_continuum_wave_76300(void);
extern uint32_t gj_batch_id_76300(void);
extern uint32_t gj_graph_milestone_76300(void);
extern const char __libcgj_batch76291_marker[];
extern const char __libcgj_batch76292_marker[];
extern const char __libcgj_batch76293_marker[];
extern const char __libcgj_batch76294_marker[];
extern const char __libcgj_batch76295_marker[];
extern const char __libcgj_batch76296_marker[];
extern const char __libcgj_batch76297_marker[];
extern const char __libcgj_batch76298_marker[];
extern const char __libcgj_batch76299_marker[];
extern const char __libcgj_batch76300_marker[];

static int
direct_check(void)
{
	int nFail = 0;

	if (gj_shell_green_76300() != 1u) {
		fprintf(stderr, "libcgj: direct gj_shell_green_76300 fail\n");
		nFail++;
	}
	if (gj_libcgj_green_76300() != 1u) {
		fprintf(stderr, "libcgj: direct gj_libcgj_green_76300 fail\n");
		nFail++;
	}
	if (gj_bar3_ready_76300() != 0u) {
		fprintf(stderr, "libcgj: direct gj_bar3_ready_76300 fail\n");
		nFail++;
	}
	if (gj_product_score_76300() != 0u) {
		fprintf(stderr, "libcgj: direct gj_product_score_76300 fail\n");
		nFail++;
	}
	if (gj_continuum_ready_76300() != 1u) {
		fprintf(stderr, "libcgj: direct gj_continuum_ready_76300 fail\n");
		nFail++;
	}
	if (gj_smoke_soft_76300() != 1u) {
		fprintf(stderr, "libcgj: direct gj_smoke_soft_76300 fail\n");
		nFail++;
	}
	if (gj_dyn_soft_76300() != 1u) {
		fprintf(stderr, "libcgj: direct gj_dyn_soft_76300 fail\n");
		nFail++;
	}
	if (gj_milestone_tag_76300() != 76300u) {
		fprintf(stderr, "libcgj: direct gj_milestone_tag_76300 fail\n");
		nFail++;
	}
	if (gj_continuum_wave_76300() != 76300u) {
		fprintf(stderr, "libcgj: direct gj_continuum_wave_76300 fail\n");
		nFail++;
	}
	if (gj_batch_id_76300() != 76300u) {
		fprintf(stderr, "libcgj: direct gj_batch_id_76300 fail\n");
		nFail++;
	}
	if (gj_graph_milestone_76300() != 76300u) {
		fprintf(stderr, "libcgj: direct gj_graph_milestone_76300 fail\n");
		nFail++;
	}
	if (strcmp(__libcgj_batch76291_marker, "libcgj-batch76291") != 0 ||
	    strcmp(__libcgj_batch76292_marker, "libcgj-batch76292") != 0 ||
	    strcmp(__libcgj_batch76293_marker, "libcgj-batch76293") != 0 ||
	    strcmp(__libcgj_batch76294_marker, "libcgj-batch76294") != 0 ||
	    strcmp(__libcgj_batch76295_marker, "libcgj-batch76295") != 0 ||
	    strcmp(__libcgj_batch76296_marker, "libcgj-batch76296") != 0 ||
	    strcmp(__libcgj_batch76297_marker, "libcgj-batch76297") != 0 ||
	    strcmp(__libcgj_batch76298_marker, "libcgj-batch76298") != 0 ||
	    strcmp(__libcgj_batch76299_marker, "libcgj-batch76299") != 0 ||
	    strcmp(__libcgj_batch76300_marker, "libcgj-batch76300") != 0) {
		fprintf(stderr, "libcgj: direct m76300 marker string fail\n");
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
		        "libcgj: soft m76300 %s KAT fail got=%u expect=%u\n",
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
		        "libcgj: soft m76300 %s marker fail got=\"%s\" "
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
#ifdef CGJ_SOFT_M76300_DIRECT
	int nFail = direct_check();
	if (nFail != 0) {
		fprintf(stderr, "libcgj: soft m76300 DIRECT FAIL fails=%d\n", nFail);
		return 1;
	}
	printf("libcgj: soft m76300 DIRECT PASS (all 11 lamps + markers)\n");
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
		fprintf(stderr, "libcgj: soft m76300 skip (dlopen fail: %s)\n",
		        dlerror());
		printf("libcgj: soft m76300 SOFT-SKIP (no product SO)\n");
		return 0;
	}

	nFail += soft_u32(h, "gj_shell_green_76300", 1u, &nChecked);
	nFail += soft_u32(h, "gj_libcgj_green_76300", 1u, &nChecked);
	nFail += soft_u32(h, "gj_bar3_ready_76300", 0u, &nChecked);
	nFail += soft_u32(h, "gj_product_score_76300", 0u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_ready_76300", 1u, &nChecked);
	nFail += soft_u32(h, "gj_smoke_soft_76300", 1u, &nChecked);
	nFail += soft_u32(h, "gj_dyn_soft_76300", 1u, &nChecked);
	nFail += soft_u32(h, "gj_milestone_tag_76300", 76300u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_wave_76300", 76300u, &nChecked);
	nFail += soft_u32(h, "gj_batch_id_76300", 76300u, &nChecked);
	nFail += soft_u32(h, "gj_graph_milestone_76300", 76300u, &nChecked);
	nFail += soft_marker(h, "__libcgj_batch76291_marker",
	                     "libcgj-batch76291", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch76292_marker",
	                     "libcgj-batch76292", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch76293_marker",
	                     "libcgj-batch76293", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch76294_marker",
	                     "libcgj-batch76294", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch76295_marker",
	                     "libcgj-batch76295", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch76296_marker",
	                     "libcgj-batch76296", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch76297_marker",
	                     "libcgj-batch76297", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch76298_marker",
	                     "libcgj-batch76298", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch76299_marker",
	                     "libcgj-batch76299", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch76300_marker",
	                     "libcgj-batch76300", &nChecked);

	if (nFail != 0) {
		(void)dlclose(h);
		fprintf(stderr,
		        "libcgj: soft m76300 FAIL path=%s fails=%d checked=%d\n",
		        szPath, nFail, nChecked);
		return 2;
	}
	printf("libcgj: soft m76300 PASS path=%s checked=%d "
	       "(soft-skip unexported/missing; bar3 still open)\n",
	       szPath, nChecked);
	(void)dlclose(h);
	return 0;
#endif
}
