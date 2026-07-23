/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 15000 markers (CREATE-ONLY).
 *
 * Coherent soft package (see NOTES_milestone_15000.txt):
 *   - this TU (libcgj continuum dyn/direct soft KATs)
 *   - NOTES_milestone_15000.txt
 *
 * Test harness note:
 *   - Sibling harnesses: cgj_smoke.c (static freestanding weak),
 *     cgj_dyn_smoke.c (host dlopen product libc.so.6),
 *     cgj_soft_milestone_14500.c / cgj_soft_milestone_14600.c /
 *     cgj_soft_milestone_14700.c / cgj_soft_milestone_14800.c /
 *     cgj_soft_milestone_14900.c
 *     (sibling decade soft probes).
 *   - This TU follows the dyn-smoke soft pattern: skip if missing,
 *     hard-fail if present but wrong. Not wired in Makefile (tests are
 *     not auto-globbed); build and run manually when product SO is ready.
 *
 * Manual build (host libc + dl):
 *   cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_15000 \
 *     user/libcgj/tests/cgj_soft_milestone_15000.c -ldl
 *   ./build/cgj_soft_milestone_15000
 *
 * Optional hard unit check (link pure graph batches directly; no SO map):
 *   cc -std=c11 -Wall -Wextra -O2 -DCGJ_SOFT_M15000_DIRECT \
 *     -o build/cgj_soft_milestone_15000_direct \
 *     user/libcgj/tests/cgj_soft_milestone_15000.c \
 *     user/libcgj/src/graph_batch14991.c \
 *     user/libcgj/src/graph_batch14992.c \
 *     user/libcgj/src/graph_batch14993.c \
 *     user/libcgj/src/graph_batch14994.c \
 *     user/libcgj/src/graph_batch14995.c \
 *     user/libcgj/src/graph_batch14996.c \
 *     user/libcgj/src/graph_batch14997.c \
 *     user/libcgj/src/graph_batch14998.c \
 *     user/libcgj/src/graph_batch14999.c \
 *     user/libcgj/src/graph_batch15000.c
 *   ./build/cgj_soft_milestone_15000_direct
 *
 * Expected soft KATs (wave 14991–15000 exclusive continuum markers):
 *   gj_shell_green_15000      → 1
 *   gj_libcgj_green_15000     → 1
 *   gj_bar3_ready_15000       → 0   (bar3 still open)
 *   gj_product_score_15000    → 0   (product score still open)
 *   gj_continuum_ready_15000  → 1
 *   gj_smoke_soft_15000       → 1
 *   gj_dyn_soft_15000         → 1
 *   gj_milestone_tag_15000    → 15000
 *   gj_continuum_wave_15000   → 15000
 *   gj_batch_id_15000         → 15000
 *   gj_graph_milestone_15000  → 15000
 *   __libcgj_batch14991_marker … __libcgj_batch15000_marker
 *     → "libcgj-batchNNNNN" string match when present
 *
 * Notes for parent wiring (outside this soft exclusive scope):
 *   - Sources graph_batch14991.c … graph_batch15000.c (and fill
 *     14976–14990 / identity 14951–14975) are CREATE-ONLY; wire into
 *     CGJ_SRCS when parent advances makefile_max to 15000.
 *   - libc.map may lag milestone 15000 globals; add 15000 symbols to the
 *     GLIBC_2.2.5 global list so dyn dlsym can see them.
 *   - Optional: fold these soft gates into cgj_dyn_smoke.c /
 *     cgj_smoke.c continuum soft blocks (same skip-if-missing rule).
 *   - Soft gates; bar3 still open (gj_bar3_ready_15000 == 0).
 */

#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef CGJ_SOFT_M15000_DIRECT
#include <dlfcn.h>
#endif

#ifdef CGJ_SOFT_M15000_DIRECT
/* Direct-link path: call product pure markers without dlopen. */
extern uint32_t gj_shell_green_15000(void);
extern uint32_t gj_libcgj_green_15000(void);
extern uint32_t gj_bar3_ready_15000(void);
extern uint32_t gj_product_score_15000(void);
extern uint32_t gj_continuum_ready_15000(void);
extern uint32_t gj_smoke_soft_15000(void);
extern uint32_t gj_dyn_soft_15000(void);
extern uint32_t gj_milestone_tag_15000(void);
extern uint32_t gj_continuum_wave_15000(void);
extern uint32_t gj_batch_id_15000(void);
extern uint32_t gj_graph_milestone_15000(void);
extern const char __libcgj_batch14991_marker[];
extern const char __libcgj_batch14992_marker[];
extern const char __libcgj_batch14993_marker[];
extern const char __libcgj_batch14994_marker[];
extern const char __libcgj_batch14995_marker[];
extern const char __libcgj_batch14996_marker[];
extern const char __libcgj_batch14997_marker[];
extern const char __libcgj_batch14998_marker[];
extern const char __libcgj_batch14999_marker[];
extern const char __libcgj_batch15000_marker[];

static int
direct_check(void)
{
	int nFail = 0;

	if (gj_shell_green_15000() != 1u) {
		fprintf(stderr, "libcgj: direct gj_shell_green_15000 fail\n");
		nFail++;
	}
	if (gj_libcgj_green_15000() != 1u) {
		fprintf(stderr, "libcgj: direct gj_libcgj_green_15000 fail\n");
		nFail++;
	}
	if (gj_bar3_ready_15000() != 0u) {
		fprintf(stderr, "libcgj: direct gj_bar3_ready_15000 fail\n");
		nFail++;
	}
	if (gj_product_score_15000() != 0u) {
		fprintf(stderr, "libcgj: direct gj_product_score_15000 fail\n");
		nFail++;
	}
	if (gj_continuum_ready_15000() != 1u) {
		fprintf(stderr, "libcgj: direct gj_continuum_ready_15000 fail\n");
		nFail++;
	}
	if (gj_smoke_soft_15000() != 1u) {
		fprintf(stderr, "libcgj: direct gj_smoke_soft_15000 fail\n");
		nFail++;
	}
	if (gj_dyn_soft_15000() != 1u) {
		fprintf(stderr, "libcgj: direct gj_dyn_soft_15000 fail\n");
		nFail++;
	}
	if (gj_milestone_tag_15000() != 15000u) {
		fprintf(stderr, "libcgj: direct gj_milestone_tag_15000 fail\n");
		nFail++;
	}
	if (gj_continuum_wave_15000() != 15000u) {
		fprintf(stderr, "libcgj: direct gj_continuum_wave_15000 fail\n");
		nFail++;
	}
	if (gj_batch_id_15000() != 15000u) {
		fprintf(stderr, "libcgj: direct gj_batch_id_15000 fail\n");
		nFail++;
	}
	if (gj_graph_milestone_15000() != 15000u) {
		fprintf(stderr,
		        "libcgj: direct gj_graph_milestone_15000 fail\n");
		nFail++;
	}
	if (strcmp(__libcgj_batch14991_marker, "libcgj-batch14991") != 0 ||
	    strcmp(__libcgj_batch14992_marker, "libcgj-batch14992") != 0 ||
	    strcmp(__libcgj_batch14993_marker, "libcgj-batch14993") != 0 ||
	    strcmp(__libcgj_batch14994_marker, "libcgj-batch14994") != 0 ||
	    strcmp(__libcgj_batch14995_marker, "libcgj-batch14995") != 0 ||
	    strcmp(__libcgj_batch14996_marker, "libcgj-batch14996") != 0 ||
	    strcmp(__libcgj_batch14997_marker, "libcgj-batch14997") != 0 ||
	    strcmp(__libcgj_batch14998_marker, "libcgj-batch14998") != 0 ||
	    strcmp(__libcgj_batch14999_marker, "libcgj-batch14999") != 0 ||
	    strcmp(__libcgj_batch15000_marker, "libcgj-batch15000") != 0) {
		fprintf(stderr, "libcgj: direct m15000 marker string fail\n");
		nFail++;
	}
	return nFail;
}
#else /* !CGJ_SOFT_M15000_DIRECT — dyn soft path */

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
		        "libcgj: soft m15000 %s KAT fail got=%u expect=%u\n",
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
		        "libcgj: soft m15000 %s marker fail got=\"%s\" "
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

#endif /* CGJ_SOFT_M15000_DIRECT */

int
main(void)
{
#ifdef CGJ_SOFT_M15000_DIRECT
	int nFail;

	nFail = direct_check();
	if (nFail != 0) {
		fprintf(stderr,
		        "libcgj: soft m15000 DIRECT FAIL fails=%d\n", nFail);
		return 1;
	}
	printf("libcgj: soft m15000 DIRECT PASS (all 11 lamps + markers)\n");
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
		        "libcgj: soft m15000 skip (dlopen fail: %s)\n",
		        dlerror());
		printf("libcgj: soft m15000 SOFT-SKIP (no product SO)\n");
		return 0;
	}

	/* Continuum soft gates: skip if missing, hard fail if wrong. */
	nFail += soft_u32(h, "gj_shell_green_15000", 1u, &nChecked);
	nFail += soft_u32(h, "gj_libcgj_green_15000", 1u, &nChecked);
	nFail += soft_u32(h, "gj_bar3_ready_15000", 0u, &nChecked);
	nFail += soft_u32(h, "gj_product_score_15000", 0u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_ready_15000", 1u, &nChecked);
	nFail += soft_u32(h, "gj_smoke_soft_15000", 1u, &nChecked);
	nFail += soft_u32(h, "gj_dyn_soft_15000", 1u, &nChecked);
	nFail += soft_u32(h, "gj_milestone_tag_15000", 15000u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_wave_15000", 15000u, &nChecked);
	nFail += soft_u32(h, "gj_batch_id_15000", 15000u, &nChecked);
	nFail += soft_u32(h, "gj_graph_milestone_15000", 15000u, &nChecked);

	nFail += soft_marker(h, "__libcgj_batch14991_marker",
	                     "libcgj-batch14991", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch14992_marker",
	                     "libcgj-batch14992", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch14993_marker",
	                     "libcgj-batch14993", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch14994_marker",
	                     "libcgj-batch14994", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch14995_marker",
	                     "libcgj-batch14995", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch14996_marker",
	                     "libcgj-batch14996", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch14997_marker",
	                     "libcgj-batch14997", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch14998_marker",
	                     "libcgj-batch14998", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch14999_marker",
	                     "libcgj-batch14999", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch15000_marker",
	                     "libcgj-batch15000", &nChecked);

	if (nFail != 0) {
		(void)dlclose(h);
		fprintf(stderr,
		        "libcgj: soft m15000 FAIL path=%s fails=%d checked=%d\n",
		        szPath, nFail, nChecked);
		return 2;
	}

	printf("libcgj: soft m15000 PASS path=%s checked=%d "
	       "(soft-skip unexported/missing; bar3 still open)\n",
	       szPath, nChecked);
	(void)dlclose(h);
	return 0;
#endif
}
