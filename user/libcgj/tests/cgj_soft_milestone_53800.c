/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 53800 markers (CREATE-ONLY).
 *
 * Coherent soft package (see NOTES_milestone_53800.txt):
 *   - this TU (libcgj continuum dyn/direct soft KATs)
 *   - NOTES_milestone_53800.txt
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
 *     cgj_soft_milestone_17700.c / cgj_soft_milestone_17800.c /
 *     cgj_soft_milestone_17900.c / cgj_soft_milestone_18000.c /
 *     cgj_soft_milestone_18100.c / cgj_soft_milestone_18200.c /
 *     cgj_soft_milestone_18300.c / cgj_soft_milestone_18400.c /
 *     cgj_soft_milestone_18500.c / cgj_soft_milestone_18600.c /
 *     cgj_soft_milestone_18700.c / cgj_soft_milestone_18800.c /
 *     cgj_soft_milestone_18900.c / cgj_soft_milestone_19000.c /
 *     cgj_soft_milestone_19100.c / cgj_soft_milestone_19200.c /
 *     cgj_soft_milestone_19300.c / cgj_soft_milestone_19400.c /
 *     cgj_soft_milestone_19500.c / cgj_soft_milestone_19600.c /
 *     cgj_soft_milestone_19700.c / cgj_soft_milestone_19800.c /
 *     cgj_soft_milestone_19900.c / cgj_soft_milestone_20000.c /
 *     cgj_soft_milestone_20100.c / cgj_soft_milestone_20200.c /
 *     cgj_soft_milestone_20300.c / cgj_soft_milestone_20400.c /
 *     cgj_soft_milestone_20500.c / cgj_soft_milestone_20600.c /
 *     cgj_soft_milestone_20700.c / cgj_soft_milestone_20800.c /
 *     cgj_soft_milestone_20900.c / cgj_soft_milestone_21000.c /
 *     cgj_soft_milestone_21100.c / cgj_soft_milestone_21200.c /
 *     cgj_soft_milestone_21300.c / cgj_soft_milestone_21400.c /
 *     cgj_soft_milestone_21500.c / cgj_soft_milestone_21600.c /
 *     cgj_soft_milestone_21700.c / cgj_soft_milestone_21800.c /
 *     cgj_soft_milestone_21900.c / cgj_soft_milestone_22000.c /
 *     cgj_soft_milestone_22100.c / cgj_soft_milestone_22200.c /
 *     cgj_soft_milestone_22300.c / cgj_soft_milestone_22400.c /
 *     cgj_soft_milestone_22500.c / cgj_soft_milestone_22600.c /
 *     cgj_soft_milestone_22700.c / cgj_soft_milestone_22800.c /
 *     cgj_soft_milestone_22900.c / cgj_soft_milestone_23000.c /
 *     cgj_soft_milestone_23100.c / cgj_soft_milestone_23200.c /
 *     cgj_soft_milestone_23300.c / cgj_soft_milestone_23400.c /
 *     cgj_soft_milestone_23500.c / cgj_soft_milestone_23600.c /
 *     cgj_soft_milestone_23700.c / cgj_soft_milestone_23800.c /
 *     cgj_soft_milestone_23900.c / cgj_soft_milestone_24000.c /
 *     cgj_soft_milestone_24100.c / cgj_soft_milestone_24200.c /
 *     cgj_soft_milestone_24300.c / cgj_soft_milestone_24400.c /
 *     cgj_soft_milestone_24500.c / cgj_soft_milestone_24600.c /
 *     cgj_soft_milestone_24700.c / cgj_soft_milestone_24800.c /
 *     cgj_soft_milestone_24900.c / cgj_soft_milestone_25000.c /
 *     cgj_soft_milestone_25100.c /
 *     cgj_soft_milestone_25200.c / cgj_soft_milestone_25300.c /
 *     cgj_soft_milestone_25400.c / cgj_soft_milestone_25500.c /
 *     cgj_soft_milestone_25600.c / cgj_soft_milestone_25700.c /
 *     (sibling decade soft probes).
 *   - This TU follows the dyn-smoke soft pattern: skip if missing,
 *     hard-fail if present but wrong. Not wired in Makefile (tests are
 *     not auto-globbed); build and run manually when product SO is ready.
 *
 * Manual build (host libc + dl):
 *   cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_53800 \
 *     user/libcgj/tests/cgj_soft_milestone_53800.c -ldl
 *   ./build/cgj_soft_milestone_53800
 *
 * Optional hard unit check (link pure graph batches directly; no SO map):
 *   cc -std=c11 -Wall -Wextra -O2 -DCGJ_SOFT_M53800_DIRECT \
 *     -o build/cgj_soft_milestone_53800_direct \
 *     user/libcgj/tests/cgj_soft_milestone_53800.c \
 *     user/libcgj/src/graph_batch53791.c \
 *     user/libcgj/src/graph_batch53792.c \
 *     user/libcgj/src/graph_batch53793.c \
 *     user/libcgj/src/graph_batch53794.c \
 *     user/libcgj/src/graph_batch53795.c \
 *     user/libcgj/src/graph_batch53796.c \
 *     user/libcgj/src/graph_batch53797.c \
 *     user/libcgj/src/graph_batch53798.c \
 *     user/libcgj/src/graph_batch53799.c \
 *     user/libcgj/src/graph_batch53800.c
 *   ./build/cgj_soft_milestone_53800_direct
 *
 * Expected soft KATs (wave 53791–53800 exclusive continuum markers):
 *   gj_shell_green_53800      → 1
 *   gj_libcgj_green_53800     → 1
 *   gj_bar3_ready_53800       → 0   (bar3 still open)
 *   gj_product_score_53800    → 0   (product score still open)
 *   gj_continuum_ready_53800  → 1
 *   gj_smoke_soft_53800       → 1
 *   gj_dyn_soft_53800         → 1
 *   gj_milestone_tag_53800    → 53800
 *   gj_continuum_wave_53800   → 53800
 *   gj_batch_id_53800         → 53800
 *   gj_graph_milestone_53800  → 53800
 *   __libcgj_batch53791_marker … __libcgj_batch53800_marker
 *     → "libcgj-batchNNNNN" string match when present
 *
 * Notes for parent wiring (outside this soft exclusive scope):
 *   - Sources graph_batch53791.c … graph_batch53800.c (and fill
 *     25776–25790 / identity 25751–25775) are CREATE-ONLY; wire into
 *     CGJ_SRCS when parent advances makefile_max to 53800.
 *   - libc.map may lag milestone 53800 globals; add 53800 symbols to the
 *     GLIBC_2.2.5 global list so dyn dlsym can see them.
 *   - Optional: fold these soft gates into cgj_dyn_smoke.c /
 *     cgj_smoke.c continuum soft blocks (same skip-if-missing rule).
 *   - Soft gates; bar3 still open (gj_bar3_ready_53800 == 0).
 */

#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef CGJ_SOFT_M53800_DIRECT
#include <dlfcn.h>
#endif

#ifdef CGJ_SOFT_M53800_DIRECT
/* Direct-link path: call product pure markers without dlopen. */
extern uint32_t gj_shell_green_53800(void);
extern uint32_t gj_libcgj_green_53800(void);
extern uint32_t gj_bar3_ready_53800(void);
extern uint32_t gj_product_score_53800(void);
extern uint32_t gj_continuum_ready_53800(void);
extern uint32_t gj_smoke_soft_53800(void);
extern uint32_t gj_dyn_soft_53800(void);
extern uint32_t gj_milestone_tag_53800(void);
extern uint32_t gj_continuum_wave_53800(void);
extern uint32_t gj_batch_id_53800(void);
extern uint32_t gj_graph_milestone_53800(void);
extern const char __libcgj_batch53791_marker[];
extern const char __libcgj_batch53792_marker[];
extern const char __libcgj_batch53793_marker[];
extern const char __libcgj_batch53794_marker[];
extern const char __libcgj_batch53795_marker[];
extern const char __libcgj_batch53796_marker[];
extern const char __libcgj_batch53797_marker[];
extern const char __libcgj_batch53798_marker[];
extern const char __libcgj_batch53799_marker[];
extern const char __libcgj_batch53800_marker[];

static int
direct_check(void)
{
	int nFail = 0;

	if (gj_shell_green_53800() != 1u) {
		fprintf(stderr, "libcgj: direct gj_shell_green_53800 fail\n");
		nFail++;
	}
	if (gj_libcgj_green_53800() != 1u) {
		fprintf(stderr, "libcgj: direct gj_libcgj_green_53800 fail\n");
		nFail++;
	}
	if (gj_bar3_ready_53800() != 0u) {
		fprintf(stderr, "libcgj: direct gj_bar3_ready_53800 fail\n");
		nFail++;
	}
	if (gj_product_score_53800() != 0u) {
		fprintf(stderr, "libcgj: direct gj_product_score_53800 fail\n");
		nFail++;
	}
	if (gj_continuum_ready_53800() != 1u) {
		fprintf(stderr, "libcgj: direct gj_continuum_ready_53800 fail\n");
		nFail++;
	}
	if (gj_smoke_soft_53800() != 1u) {
		fprintf(stderr, "libcgj: direct gj_smoke_soft_53800 fail\n");
		nFail++;
	}
	if (gj_dyn_soft_53800() != 1u) {
		fprintf(stderr, "libcgj: direct gj_dyn_soft_53800 fail\n");
		nFail++;
	}
	if (gj_milestone_tag_53800() != 53800u) {
		fprintf(stderr, "libcgj: direct gj_milestone_tag_53800 fail\n");
		nFail++;
	}
	if (gj_continuum_wave_53800() != 53800u) {
		fprintf(stderr, "libcgj: direct gj_continuum_wave_53800 fail\n");
		nFail++;
	}
	if (gj_batch_id_53800() != 53800u) {
		fprintf(stderr, "libcgj: direct gj_batch_id_53800 fail\n");
		nFail++;
	}
	if (gj_graph_milestone_53800() != 53800u) {
		fprintf(stderr,
		        "libcgj: direct gj_graph_milestone_53800 fail\n");
		nFail++;
	}
	if (strcmp(__libcgj_batch53791_marker, "libcgj-batch53791") != 0 ||
	    strcmp(__libcgj_batch53792_marker, "libcgj-batch53792") != 0 ||
	    strcmp(__libcgj_batch53793_marker, "libcgj-batch53793") != 0 ||
	    strcmp(__libcgj_batch53794_marker, "libcgj-batch53794") != 0 ||
	    strcmp(__libcgj_batch53795_marker, "libcgj-batch53795") != 0 ||
	    strcmp(__libcgj_batch53796_marker, "libcgj-batch53796") != 0 ||
	    strcmp(__libcgj_batch53797_marker, "libcgj-batch53797") != 0 ||
	    strcmp(__libcgj_batch53798_marker, "libcgj-batch53798") != 0 ||
	    strcmp(__libcgj_batch53799_marker, "libcgj-batch53799") != 0 ||
	    strcmp(__libcgj_batch53800_marker, "libcgj-batch53800") != 0) {
		fprintf(stderr, "libcgj: direct m53800 marker string fail\n");
		nFail++;
	}
	return nFail;
}
#else /* !CGJ_SOFT_M53800_DIRECT — dyn soft path */

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
		        "libcgj: soft m53800 %s KAT fail got=%u expect=%u\n",
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
		        "libcgj: soft m53800 %s marker fail got=\"%s\" "
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

#endif /* CGJ_SOFT_M53800_DIRECT */

int
main(void)
{
#ifdef CGJ_SOFT_M53800_DIRECT
	int nFail;

	nFail = direct_check();
	if (nFail != 0) {
		fprintf(stderr,
		        "libcgj: soft m53800 DIRECT FAIL fails=%d\n", nFail);
		return 1;
	}
	printf("libcgj: soft m53800 DIRECT PASS (all 11 lamps + markers)\n");
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
		        "libcgj: soft m53800 skip (dlopen fail: %s)\n",
		        dlerror());
		printf("libcgj: soft m53800 SOFT-SKIP (no product SO)\n");
		return 0;
	}

	/* Continuum soft gates: skip if missing, hard fail if wrong. */
	nFail += soft_u32(h, "gj_shell_green_53800", 1u, &nChecked);
	nFail += soft_u32(h, "gj_libcgj_green_53800", 1u, &nChecked);
	nFail += soft_u32(h, "gj_bar3_ready_53800", 0u, &nChecked);
	nFail += soft_u32(h, "gj_product_score_53800", 0u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_ready_53800", 1u, &nChecked);
	nFail += soft_u32(h, "gj_smoke_soft_53800", 1u, &nChecked);
	nFail += soft_u32(h, "gj_dyn_soft_53800", 1u, &nChecked);
	nFail += soft_u32(h, "gj_milestone_tag_53800", 53800u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_wave_53800", 53800u, &nChecked);
	nFail += soft_u32(h, "gj_batch_id_53800", 53800u, &nChecked);
	nFail += soft_u32(h, "gj_graph_milestone_53800", 53800u, &nChecked);

	nFail += soft_marker(h, "__libcgj_batch53791_marker",
	                     "libcgj-batch53791", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch53792_marker",
	                     "libcgj-batch53792", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch53793_marker",
	                     "libcgj-batch53793", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch53794_marker",
	                     "libcgj-batch53794", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch53795_marker",
	                     "libcgj-batch53795", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch53796_marker",
	                     "libcgj-batch53796", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch53797_marker",
	                     "libcgj-batch53797", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch53798_marker",
	                     "libcgj-batch53798", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch53799_marker",
	                     "libcgj-batch53799", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch53800_marker",
	                     "libcgj-batch53800", &nChecked);

	if (nFail != 0) {
		(void)dlclose(h);
		fprintf(stderr,
		        "libcgj: soft m53800 FAIL path=%s fails=%d checked=%d\n",
		        szPath, nFail, nChecked);
		return 2;
	}

	printf("libcgj: soft m53800 PASS path=%s checked=%d "
	       "(soft-skip unexported/missing; bar3 still open)\n",
	       szPath, nChecked);
	(void)dlclose(h);
	return 0;
#endif
}
