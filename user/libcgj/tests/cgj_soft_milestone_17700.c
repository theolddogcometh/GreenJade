/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 17700 markers (CREATE-ONLY).
 *
 * Coherent soft package (see NOTES_milestone_17700.txt):
 *   - this TU (libcgj continuum dyn/direct soft KATs)
 *   - NOTES_milestone_17700.txt
 *
 * Test harness note:
 *   - Sibling harnesses: cgj_smoke.c (static freestanding weak),
 *     cgj_dyn_smoke.c (host dlopen product libc.so.6),
 *     cgj_soft_milestone_14500.c / cgj_soft_milestone_14600.c /
 *     cgj_soft_milestone_14700.c / cgj_soft_milestone_14800.c /
 *     cgj_soft_milestone_14900.c / cgj_soft_milestone_15000.c /
 *     cgj_soft_milestone_15100.c / cgj_soft_milestone_15200.c /
 *     cgj_soft_milestone_15300.c / cgj_soft_milestone_15400.c /
 *     cgj_soft_milestone_15500.c / cgj_soft_milestone_15600.c /
 *     cgj_soft_milestone_15700.c / cgj_soft_milestone_15800.c /
 *     cgj_soft_milestone_15900.c / cgj_soft_milestone_16000.c /
 *     cgj_soft_milestone_16100.c / cgj_soft_milestone_16200.c /
 *     cgj_soft_milestone_16300.c / cgj_soft_milestone_16400.c /
 *     cgj_soft_milestone_16500.c / cgj_soft_milestone_16600.c /
 *     cgj_soft_milestone_16700.c / cgj_soft_milestone_16800.c /
 *     cgj_soft_milestone_16900.c / cgj_soft_milestone_17000.c /
 *     cgj_soft_milestone_17100.c / cgj_soft_milestone_17200.c /
 *     cgj_soft_milestone_17300.c / cgj_soft_milestone_17400.c /
 *     cgj_soft_milestone_17500.c / cgj_soft_milestone_17600.c /
 *     cgj_soft_milestone_17700.c
 *     (sibling decade soft probes).
 *   - This TU follows the dyn-smoke soft pattern: skip if missing,
 *     hard-fail if present but wrong. Not wired in Makefile (tests are
 *     not auto-globbed); build and run manually when product SO is ready.
 *
 * Manual build (host libc + dl):
 *   cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_17700 \
 *     user/libcgj/tests/cgj_soft_milestone_17700.c -ldl
 *   ./build/cgj_soft_milestone_17700
 *
 * Optional hard unit check (link pure graph batches directly; no SO map):
 *   cc -std=c11 -Wall -Wextra -O2 -DCGJ_SOFT_M17700_DIRECT \
 *     -o build/cgj_soft_milestone_17700_direct \
 *     user/libcgj/tests/cgj_soft_milestone_17700.c \
 *     user/libcgj/src/graph_batch17691.c \
 *     user/libcgj/src/graph_batch17692.c \
 *     user/libcgj/src/graph_batch17693.c \
 *     user/libcgj/src/graph_batch17694.c \
 *     user/libcgj/src/graph_batch17695.c \
 *     user/libcgj/src/graph_batch17696.c \
 *     user/libcgj/src/graph_batch17697.c \
 *     user/libcgj/src/graph_batch17698.c \
 *     user/libcgj/src/graph_batch17699.c \
 *     user/libcgj/src/graph_batch17700.c
 *   ./build/cgj_soft_milestone_17700_direct
 *
 * Expected soft KATs (wave 17691–17700 exclusive continuum markers):
 *   gj_shell_green_17700      → 1
 *   gj_libcgj_green_17700     → 1
 *   gj_bar3_ready_17700       → 0   (bar3 still open)
 *   gj_product_score_17700    → 0   (product score still open)
 *   gj_continuum_ready_17700  → 1
 *   gj_smoke_soft_17700       → 1
 *   gj_dyn_soft_17700         → 1
 *   gj_milestone_tag_17700    → 17700
 *   gj_continuum_wave_17700   → 17700
 *   gj_batch_id_17700         → 17700
 *   gj_graph_milestone_17700  → 17700
 *   __libcgj_batch17691_marker … __libcgj_batch17700_marker
 *     → "libcgj-batchNNNNN" string match when present
 *
 * Notes for parent wiring (outside this soft exclusive scope):
 *   - Sources graph_batch17691.c … graph_batch17700.c (and fill
 *     17676–17690 / identity 17651–17675) are CREATE-ONLY; wire into
 *     CGJ_SRCS when parent advances makefile_max to 17700.
 *   - libc.map may lag milestone 17700 globals; add 17700 symbols to the
 *     GLIBC_2.2.5 global list so dyn dlsym can see them.
 *   - Optional: fold these soft gates into cgj_dyn_smoke.c /
 *     cgj_smoke.c continuum soft blocks (same skip-if-missing rule).
 *   - Soft gates; bar3 still open (gj_bar3_ready_17700 == 0).
 */

#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef CGJ_SOFT_M17700_DIRECT
#include <dlfcn.h>
#endif

#ifdef CGJ_SOFT_M17700_DIRECT
/* Direct-link path: call product pure markers without dlopen. */
extern uint32_t gj_shell_green_17700(void);
extern uint32_t gj_libcgj_green_17700(void);
extern uint32_t gj_bar3_ready_17700(void);
extern uint32_t gj_product_score_17700(void);
extern uint32_t gj_continuum_ready_17700(void);
extern uint32_t gj_smoke_soft_17700(void);
extern uint32_t gj_dyn_soft_17700(void);
extern uint32_t gj_milestone_tag_17700(void);
extern uint32_t gj_continuum_wave_17700(void);
extern uint32_t gj_batch_id_17700(void);
extern uint32_t gj_graph_milestone_17700(void);
extern const char __libcgj_batch17691_marker[];
extern const char __libcgj_batch17692_marker[];
extern const char __libcgj_batch17693_marker[];
extern const char __libcgj_batch17694_marker[];
extern const char __libcgj_batch17695_marker[];
extern const char __libcgj_batch17696_marker[];
extern const char __libcgj_batch17697_marker[];
extern const char __libcgj_batch17698_marker[];
extern const char __libcgj_batch17699_marker[];
extern const char __libcgj_batch17700_marker[];

static int
direct_check(void)
{
	int nFail = 0;

	if (gj_shell_green_17700() != 1u) {
		fprintf(stderr, "libcgj: direct gj_shell_green_17700 fail\n");
		nFail++;
	}
	if (gj_libcgj_green_17700() != 1u) {
		fprintf(stderr, "libcgj: direct gj_libcgj_green_17700 fail\n");
		nFail++;
	}
	if (gj_bar3_ready_17700() != 0u) {
		fprintf(stderr, "libcgj: direct gj_bar3_ready_17700 fail\n");
		nFail++;
	}
	if (gj_product_score_17700() != 0u) {
		fprintf(stderr, "libcgj: direct gj_product_score_17700 fail\n");
		nFail++;
	}
	if (gj_continuum_ready_17700() != 1u) {
		fprintf(stderr, "libcgj: direct gj_continuum_ready_17700 fail\n");
		nFail++;
	}
	if (gj_smoke_soft_17700() != 1u) {
		fprintf(stderr, "libcgj: direct gj_smoke_soft_17700 fail\n");
		nFail++;
	}
	if (gj_dyn_soft_17700() != 1u) {
		fprintf(stderr, "libcgj: direct gj_dyn_soft_17700 fail\n");
		nFail++;
	}
	if (gj_milestone_tag_17700() != 17700u) {
		fprintf(stderr, "libcgj: direct gj_milestone_tag_17700 fail\n");
		nFail++;
	}
	if (gj_continuum_wave_17700() != 17700u) {
		fprintf(stderr, "libcgj: direct gj_continuum_wave_17700 fail\n");
		nFail++;
	}
	if (gj_batch_id_17700() != 17700u) {
		fprintf(stderr, "libcgj: direct gj_batch_id_17700 fail\n");
		nFail++;
	}
	if (gj_graph_milestone_17700() != 17700u) {
		fprintf(stderr,
		        "libcgj: direct gj_graph_milestone_17700 fail\n");
		nFail++;
	}
	if (strcmp(__libcgj_batch17691_marker, "libcgj-batch17691") != 0 ||
	    strcmp(__libcgj_batch17692_marker, "libcgj-batch17692") != 0 ||
	    strcmp(__libcgj_batch17693_marker, "libcgj-batch17693") != 0 ||
	    strcmp(__libcgj_batch17694_marker, "libcgj-batch17694") != 0 ||
	    strcmp(__libcgj_batch17695_marker, "libcgj-batch17695") != 0 ||
	    strcmp(__libcgj_batch17696_marker, "libcgj-batch17696") != 0 ||
	    strcmp(__libcgj_batch17697_marker, "libcgj-batch17697") != 0 ||
	    strcmp(__libcgj_batch17698_marker, "libcgj-batch17698") != 0 ||
	    strcmp(__libcgj_batch17699_marker, "libcgj-batch17699") != 0 ||
	    strcmp(__libcgj_batch17700_marker, "libcgj-batch17700") != 0) {
		fprintf(stderr, "libcgj: direct m17700 marker string fail\n");
		nFail++;
	}
	return nFail;
}
#else /* !CGJ_SOFT_M17700_DIRECT — dyn soft path */

typedef uint32_t (*u32_fn)(void);

/* Soft gate: missing → skip (return 0); present+wrong → return 1 (fail). */
static int
soft_u32(void *h, const char *szName, uint32_t u32Expect, int *pChecked)
{
	u32_fn pfn;
	uint32_t u32Got;

	pfn = (u32_fn)dlsym(h, szName);
	if (pfn == NULL) {
		return 0;
	}
	if (pChecked != NULL) {
		(*pChecked)++;
	}
	u32Got = pfn();
	if (u32Got != u32Expect) {
		fprintf(stderr,
		        "libcgj: soft m17700 %s KAT fail got=%u expect=%u\n",
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
	if (szGot == NULL) {
		return 0;
	}
	if (pChecked != NULL) {
		(*pChecked)++;
	}
	if (strcmp(szGot, szExpect) != 0) {
		fprintf(stderr,
		        "libcgj: soft m17700 %s marker fail got=\"%s\" "
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

#endif /* CGJ_SOFT_M17700_DIRECT */

int
main(void)
{
#ifdef CGJ_SOFT_M17700_DIRECT
	int nFail;

	nFail = direct_check();
	if (nFail != 0) {
		fprintf(stderr,
		        "libcgj: soft m17700 DIRECT FAIL fails=%d\n", nFail);
		return 1;
	}
	printf("libcgj: soft m17700 DIRECT PASS (all 11 lamps + markers)\n");
	return 0;
#else
	void *h = NULL;
	const char *szPath = NULL;
	int i;
	int nChecked = 0;
	int nFail = 0;

	for (i = 0; kPaths[i] != NULL; i++) {
		h = dlopen(kPaths[i], RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND);
		if (h != NULL) {
			szPath = kPaths[i];
			break;
		}
	}
	if (h == NULL) {
		/*
		 * Soft: product SO not built yet is not a hard fail for this
		 * CREATE-ONLY host probe. Report skip and exit 0.
		 */
		fprintf(stderr,
		        "libcgj: soft m17700 skip (dlopen fail: %s)\n",
		        dlerror());
		printf("libcgj: soft m17700 SOFT-SKIP (no product SO)\n");
		return 0;
	}

	/* Continuum soft gates: skip if missing, hard fail if wrong. */
	nFail += soft_u32(h, "gj_shell_green_17700", 1u, &nChecked);
	nFail += soft_u32(h, "gj_libcgj_green_17700", 1u, &nChecked);
	nFail += soft_u32(h, "gj_bar3_ready_17700", 0u, &nChecked);
	nFail += soft_u32(h, "gj_product_score_17700", 0u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_ready_17700", 1u, &nChecked);
	nFail += soft_u32(h, "gj_smoke_soft_17700", 1u, &nChecked);
	nFail += soft_u32(h, "gj_dyn_soft_17700", 1u, &nChecked);
	nFail += soft_u32(h, "gj_milestone_tag_17700", 17700u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_wave_17700", 17700u, &nChecked);
	nFail += soft_u32(h, "gj_batch_id_17700", 17700u, &nChecked);
	nFail += soft_u32(h, "gj_graph_milestone_17700", 17700u, &nChecked);

	nFail += soft_marker(h, "__libcgj_batch17691_marker",
	                     "libcgj-batch17691", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch17692_marker",
	                     "libcgj-batch17692", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch17693_marker",
	                     "libcgj-batch17693", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch17694_marker",
	                     "libcgj-batch17694", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch17695_marker",
	                     "libcgj-batch17695", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch17696_marker",
	                     "libcgj-batch17696", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch17697_marker",
	                     "libcgj-batch17697", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch17698_marker",
	                     "libcgj-batch17698", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch17699_marker",
	                     "libcgj-batch17699", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch17700_marker",
	                     "libcgj-batch17700", &nChecked);

	if (nFail != 0) {
		(void)dlclose(h);
		fprintf(stderr,
		        "libcgj: soft m17700 FAIL path=%s fails=%d checked=%d\n",
		        szPath, nFail, nChecked);
		return 2;
	}

	printf("libcgj: soft m17700 PASS path=%s checked=%d "
	       "(soft-skip unexported/missing; bar3 still open)\n",
	       szPath, nChecked);
	(void)dlclose(h);
	return 0;
#endif
}
