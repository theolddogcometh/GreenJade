/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 14700 markers (CREATE-ONLY).
 *
 * Coherent soft package (see NOTES_milestone_14700.txt):
 *   - this TU (libcgj continuum dyn/direct soft KATs)
 *   - NOTES_milestone_14700.txt
 *   - user/libgj-so/src/graph_so_batch1.c /
 *     user/libgj-so/src/graph_so_batch2.c (companion SysV SO soft;
 *     freestanding compile-check only; not dlopen'd here)
 *
 * Test harness note:
 *   - Sibling harnesses: cgj_smoke.c (static freestanding weak),
 *     cgj_dyn_smoke.c (host dlopen product libc.so.6),
 *     cgj_soft_milestone_14500.c / cgj_soft_milestone_14600.c /
 *     cgj_soft_milestone_14800.c / cgj_soft_milestone_14900.c
 *     (sibling decade soft probes).
 *   - NOTES_milestone_14600 deferred this probe until CREATE-ONLY batch
 *     TUs graph_batch14691.c … graph_batch14700.c landed; they have
 *     and are already listed on CGJ_SRCS.
 *   - This TU follows the dyn-smoke soft pattern: skip if missing,
 *     hard-fail if present but wrong. Not wired in Makefile (tests are
 *     not auto-globbed); build and run manually when product SO is ready.
 *
 * Manual build (host libc + dl):
 *   cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_14700 \
 *     user/libcgj/tests/cgj_soft_milestone_14700.c -ldl
 *   ./build/cgj_soft_milestone_14700
 *
 * Optional hard unit check (link pure graph batches directly; no SO map):
 *   cc -std=c11 -Wall -Wextra -O2 -DCGJ_SOFT_M14700_DIRECT \
 *     -o build/cgj_soft_milestone_14700_direct \
 *     user/libcgj/tests/cgj_soft_milestone_14700.c \
 *     user/libcgj/src/graph_batch14691.c \
 *     user/libcgj/src/graph_batch14692.c \
 *     user/libcgj/src/graph_batch14693.c \
 *     user/libcgj/src/graph_batch14694.c \
 *     user/libcgj/src/graph_batch14695.c \
 *     user/libcgj/src/graph_batch14696.c \
 *     user/libcgj/src/graph_batch14697.c \
 *     user/libcgj/src/graph_batch14698.c \
 *     user/libcgj/src/graph_batch14699.c \
 *     user/libcgj/src/graph_batch14700.c
 *   ./build/cgj_soft_milestone_14700_direct
 *
 * Expected soft KATs (wave 14691–14700 exclusive continuum markers):
 *   gj_shell_green_14700      → 1
 *   gj_libcgj_green_14700     → 1
 *   gj_bar3_ready_14700       → 0   (bar3 still open)
 *   gj_product_score_14700    → 0   (product score still open)
 *   gj_continuum_ready_14700  → 1
 *   gj_smoke_soft_14700       → 1
 *   gj_dyn_soft_14700         → 1
 *   gj_milestone_tag_14700    → 14700
 *   gj_continuum_wave_14700   → 14700
 *   gj_batch_id_14700         → 14700
 *   gj_graph_milestone_14700  → 14700
 *   __libcgj_batch14691_marker … __libcgj_batch14700_marker
 *     → "libcgj-batchNNNNN" string match when present
 *
 * Notes for parent wiring (outside this soft exclusive scope):
 *   - graph_batch14691.c … graph_batch14700.c are already on CGJ_SRCS;
 *     rebuild libcgj / libc.so.6 may still lag local object trees.
 *   - libc.map may lag milestone 14700 globals; add 14700 symbols to the
 *     GLIBC_2.2.5 global list so dyn dlsym can see them.
 *   - Optional: fold these soft gates into cgj_dyn_smoke.c /
 *     cgj_smoke.c continuum soft blocks (same skip-if-missing rule).
 *   - Soft gates; bar3 still open (gj_bar3_ready_14700 == 0).
 *   - Companion libgj-so soft batches (graph_so_batch1/2) stay unwired;
 *     product SO is still libgj_so.c only (see NOTES_milestone_14700.txt).
 */

#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef CGJ_SOFT_M14700_DIRECT
#include <dlfcn.h>
#endif

#ifdef CGJ_SOFT_M14700_DIRECT
/* Direct-link path: call product pure markers without dlopen. */
extern uint32_t gj_shell_green_14700(void);
extern uint32_t gj_libcgj_green_14700(void);
extern uint32_t gj_bar3_ready_14700(void);
extern uint32_t gj_product_score_14700(void);
extern uint32_t gj_continuum_ready_14700(void);
extern uint32_t gj_smoke_soft_14700(void);
extern uint32_t gj_dyn_soft_14700(void);
extern uint32_t gj_milestone_tag_14700(void);
extern uint32_t gj_continuum_wave_14700(void);
extern uint32_t gj_batch_id_14700(void);
extern uint32_t gj_graph_milestone_14700(void);
extern const char __libcgj_batch14691_marker[];
extern const char __libcgj_batch14692_marker[];
extern const char __libcgj_batch14693_marker[];
extern const char __libcgj_batch14694_marker[];
extern const char __libcgj_batch14695_marker[];
extern const char __libcgj_batch14696_marker[];
extern const char __libcgj_batch14697_marker[];
extern const char __libcgj_batch14698_marker[];
extern const char __libcgj_batch14699_marker[];
extern const char __libcgj_batch14700_marker[];

static int
direct_check(void)
{
	int nFail = 0;

	if (gj_shell_green_14700() != 1u) {
		fprintf(stderr, "libcgj: direct gj_shell_green_14700 fail\n");
		nFail++;
	}
	if (gj_libcgj_green_14700() != 1u) {
		fprintf(stderr, "libcgj: direct gj_libcgj_green_14700 fail\n");
		nFail++;
	}
	if (gj_bar3_ready_14700() != 0u) {
		fprintf(stderr, "libcgj: direct gj_bar3_ready_14700 fail\n");
		nFail++;
	}
	if (gj_product_score_14700() != 0u) {
		fprintf(stderr, "libcgj: direct gj_product_score_14700 fail\n");
		nFail++;
	}
	if (gj_continuum_ready_14700() != 1u) {
		fprintf(stderr, "libcgj: direct gj_continuum_ready_14700 fail\n");
		nFail++;
	}
	if (gj_smoke_soft_14700() != 1u) {
		fprintf(stderr, "libcgj: direct gj_smoke_soft_14700 fail\n");
		nFail++;
	}
	if (gj_dyn_soft_14700() != 1u) {
		fprintf(stderr, "libcgj: direct gj_dyn_soft_14700 fail\n");
		nFail++;
	}
	if (gj_milestone_tag_14700() != 14700u) {
		fprintf(stderr, "libcgj: direct gj_milestone_tag_14700 fail\n");
		nFail++;
	}
	if (gj_continuum_wave_14700() != 14700u) {
		fprintf(stderr, "libcgj: direct gj_continuum_wave_14700 fail\n");
		nFail++;
	}
	if (gj_batch_id_14700() != 14700u) {
		fprintf(stderr, "libcgj: direct gj_batch_id_14700 fail\n");
		nFail++;
	}
	if (gj_graph_milestone_14700() != 14700u) {
		fprintf(stderr,
		        "libcgj: direct gj_graph_milestone_14700 fail\n");
		nFail++;
	}
	if (strcmp(__libcgj_batch14691_marker, "libcgj-batch14691") != 0 ||
	    strcmp(__libcgj_batch14692_marker, "libcgj-batch14692") != 0 ||
	    strcmp(__libcgj_batch14693_marker, "libcgj-batch14693") != 0 ||
	    strcmp(__libcgj_batch14694_marker, "libcgj-batch14694") != 0 ||
	    strcmp(__libcgj_batch14695_marker, "libcgj-batch14695") != 0 ||
	    strcmp(__libcgj_batch14696_marker, "libcgj-batch14696") != 0 ||
	    strcmp(__libcgj_batch14697_marker, "libcgj-batch14697") != 0 ||
	    strcmp(__libcgj_batch14698_marker, "libcgj-batch14698") != 0 ||
	    strcmp(__libcgj_batch14699_marker, "libcgj-batch14699") != 0 ||
	    strcmp(__libcgj_batch14700_marker, "libcgj-batch14700") != 0) {
		fprintf(stderr, "libcgj: direct m14700 marker string fail\n");
		nFail++;
	}
	return nFail;
}
#else /* !CGJ_SOFT_M14700_DIRECT — dyn soft path */

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
		        "libcgj: soft m14700 %s KAT fail got=%u expect=%u\n",
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
		        "libcgj: soft m14700 %s marker fail got=\"%s\" "
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

#endif /* CGJ_SOFT_M14700_DIRECT */

int
main(void)
{
#ifdef CGJ_SOFT_M14700_DIRECT
	int nFail;

	nFail = direct_check();
	if (nFail != 0) {
		fprintf(stderr,
		        "libcgj: soft m14700 DIRECT FAIL fails=%d\n", nFail);
		return 1;
	}
	printf("libcgj: soft m14700 DIRECT PASS (all 11 lamps + markers)\n");
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
		        "libcgj: soft m14700 skip (dlopen fail: %s)\n",
		        dlerror());
		printf("libcgj: soft m14700 SOFT-SKIP (no product SO)\n");
		return 0;
	}

	/* Continuum soft gates: skip if missing, hard fail if wrong. */
	nFail += soft_u32(h, "gj_shell_green_14700", 1u, &nChecked);
	nFail += soft_u32(h, "gj_libcgj_green_14700", 1u, &nChecked);
	nFail += soft_u32(h, "gj_bar3_ready_14700", 0u, &nChecked);
	nFail += soft_u32(h, "gj_product_score_14700", 0u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_ready_14700", 1u, &nChecked);
	nFail += soft_u32(h, "gj_smoke_soft_14700", 1u, &nChecked);
	nFail += soft_u32(h, "gj_dyn_soft_14700", 1u, &nChecked);
	nFail += soft_u32(h, "gj_milestone_tag_14700", 14700u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_wave_14700", 14700u, &nChecked);
	nFail += soft_u32(h, "gj_batch_id_14700", 14700u, &nChecked);
	nFail += soft_u32(h, "gj_graph_milestone_14700", 14700u, &nChecked);

	nFail += soft_marker(h, "__libcgj_batch14691_marker",
	                     "libcgj-batch14691", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch14692_marker",
	                     "libcgj-batch14692", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch14693_marker",
	                     "libcgj-batch14693", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch14694_marker",
	                     "libcgj-batch14694", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch14695_marker",
	                     "libcgj-batch14695", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch14696_marker",
	                     "libcgj-batch14696", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch14697_marker",
	                     "libcgj-batch14697", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch14698_marker",
	                     "libcgj-batch14698", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch14699_marker",
	                     "libcgj-batch14699", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch14700_marker",
	                     "libcgj-batch14700", &nChecked);

	if (nFail != 0) {
		(void)dlclose(h);
		fprintf(stderr,
		        "libcgj: soft m14700 FAIL path=%s fails=%d checked=%d\n",
		        szPath, nFail, nChecked);
		return 2;
	}

	printf("libcgj: soft m14700 PASS path=%s checked=%d "
	       "(soft-skip unexported/missing; bar3 still open)\n",
	       szPath, nChecked);
	(void)dlclose(h);
	return 0;
#endif
}
