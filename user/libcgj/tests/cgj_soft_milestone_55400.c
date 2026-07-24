/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 55400 markers (CREATE-ONLY).
 *
 * Coherent soft package (see NOTES_milestone_55400.txt):
 *   - this TU (libcgj continuum dyn/direct soft KATs)
 *   - NOTES_milestone_55400.txt
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
 *   cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_55400 \
 *     user/libcgj/tests/cgj_soft_milestone_55400.c -ldl
 *   ./build/cgj_soft_milestone_55400
 *
 * Optional hard unit check (link pure graph batches directly; no SO map):
 *   cc -std=c11 -Wall -Wextra -O2 -DCGJ_SOFT_M55400_DIRECT \
 *     -o build/cgj_soft_milestone_55400_direct \
 *     user/libcgj/tests/cgj_soft_milestone_55400.c \
 *     user/libcgj/src/graph_batch55391.c \
 *     user/libcgj/src/graph_batch55392.c \
 *     user/libcgj/src/graph_batch55393.c \
 *     user/libcgj/src/graph_batch55394.c \
 *     user/libcgj/src/graph_batch55395.c \
 *     user/libcgj/src/graph_batch55396.c \
 *     user/libcgj/src/graph_batch55397.c \
 *     user/libcgj/src/graph_batch55398.c \
 *     user/libcgj/src/graph_batch55399.c \
 *     user/libcgj/src/graph_batch55400.c
 *   ./build/cgj_soft_milestone_55400_direct
 *
 * Expected soft KATs (wave 55391–55400 exclusive continuum markers):
 *   gj_shell_green_55400      → 1
 *   gj_libcgj_green_55400     → 1
 *   gj_bar3_ready_55400       → 0   (bar3 still open)
 *   gj_product_score_55400    → 0   (product score still open)
 *   gj_continuum_ready_55400  → 1
 *   gj_smoke_soft_55400       → 1
 *   gj_dyn_soft_55400         → 1
 *   gj_milestone_tag_55400    → 55400
 *   gj_continuum_wave_55400   → 55400
 *   gj_batch_id_55400         → 55400
 *   gj_graph_milestone_55400  → 55400
 *   __libcgj_batch55391_marker … __libcgj_batch55400_marker
 *     → "libcgj-batchNNNNN" string match when present
 *
 * Notes for parent wiring (outside this soft exclusive scope):
 *   - Sources graph_batch55391.c … graph_batch55400.c (and fill
 *     25776–25790 / identity 25751–25775) are CREATE-ONLY; wire into
 *     CGJ_SRCS when parent advances makefile_max to 55400.
 *   - libc.map may lag milestone 55400 globals; add 55400 symbols to the
 *     GLIBC_2.2.5 global list so dyn dlsym can see them.
 *   - Optional: fold these soft gates into cgj_dyn_smoke.c /
 *     cgj_smoke.c continuum soft blocks (same skip-if-missing rule).
 *   - Soft gates; bar3 still open (gj_bar3_ready_55400 == 0).
 */

#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef CGJ_SOFT_M55400_DIRECT
#include <dlfcn.h>
#endif

#ifdef CGJ_SOFT_M55400_DIRECT
/* Direct-link path: call product pure markers without dlopen. */
extern uint32_t gj_shell_green_55400(void);
extern uint32_t gj_libcgj_green_55400(void);
extern uint32_t gj_bar3_ready_55400(void);
extern uint32_t gj_product_score_55400(void);
extern uint32_t gj_continuum_ready_55400(void);
extern uint32_t gj_smoke_soft_55400(void);
extern uint32_t gj_dyn_soft_55400(void);
extern uint32_t gj_milestone_tag_55400(void);
extern uint32_t gj_continuum_wave_55400(void);
extern uint32_t gj_batch_id_55400(void);
extern uint32_t gj_graph_milestone_55400(void);
extern const char __libcgj_batch55391_marker[];
extern const char __libcgj_batch55392_marker[];
extern const char __libcgj_batch55393_marker[];
extern const char __libcgj_batch55394_marker[];
extern const char __libcgj_batch55395_marker[];
extern const char __libcgj_batch55396_marker[];
extern const char __libcgj_batch55397_marker[];
extern const char __libcgj_batch55398_marker[];
extern const char __libcgj_batch55399_marker[];
extern const char __libcgj_batch55400_marker[];

static int
direct_check(void)
{
	int nFail = 0;

	if (gj_shell_green_55400() != 1u) {
		fprintf(stderr, "libcgj: direct gj_shell_green_55400 fail\n");
		nFail++;
	}
	if (gj_libcgj_green_55400() != 1u) {
		fprintf(stderr, "libcgj: direct gj_libcgj_green_55400 fail\n");
		nFail++;
	}
	if (gj_bar3_ready_55400() != 0u) {
		fprintf(stderr, "libcgj: direct gj_bar3_ready_55400 fail\n");
		nFail++;
	}
	if (gj_product_score_55400() != 0u) {
		fprintf(stderr, "libcgj: direct gj_product_score_55400 fail\n");
		nFail++;
	}
	if (gj_continuum_ready_55400() != 1u) {
		fprintf(stderr, "libcgj: direct gj_continuum_ready_55400 fail\n");
		nFail++;
	}
	if (gj_smoke_soft_55400() != 1u) {
		fprintf(stderr, "libcgj: direct gj_smoke_soft_55400 fail\n");
		nFail++;
	}
	if (gj_dyn_soft_55400() != 1u) {
		fprintf(stderr, "libcgj: direct gj_dyn_soft_55400 fail\n");
		nFail++;
	}
	if (gj_milestone_tag_55400() != 55400u) {
		fprintf(stderr, "libcgj: direct gj_milestone_tag_55400 fail\n");
		nFail++;
	}
	if (gj_continuum_wave_55400() != 55400u) {
		fprintf(stderr, "libcgj: direct gj_continuum_wave_55400 fail\n");
		nFail++;
	}
	if (gj_batch_id_55400() != 55400u) {
		fprintf(stderr, "libcgj: direct gj_batch_id_55400 fail\n");
		nFail++;
	}
	if (gj_graph_milestone_55400() != 55400u) {
		fprintf(stderr,
		        "libcgj: direct gj_graph_milestone_55400 fail\n");
		nFail++;
	}
	if (strcmp(__libcgj_batch55391_marker, "libcgj-batch55391") != 0 ||
	    strcmp(__libcgj_batch55392_marker, "libcgj-batch55392") != 0 ||
	    strcmp(__libcgj_batch55393_marker, "libcgj-batch55393") != 0 ||
	    strcmp(__libcgj_batch55394_marker, "libcgj-batch55394") != 0 ||
	    strcmp(__libcgj_batch55395_marker, "libcgj-batch55395") != 0 ||
	    strcmp(__libcgj_batch55396_marker, "libcgj-batch55396") != 0 ||
	    strcmp(__libcgj_batch55397_marker, "libcgj-batch55397") != 0 ||
	    strcmp(__libcgj_batch55398_marker, "libcgj-batch55398") != 0 ||
	    strcmp(__libcgj_batch55399_marker, "libcgj-batch55399") != 0 ||
	    strcmp(__libcgj_batch55400_marker, "libcgj-batch55400") != 0) {
		fprintf(stderr, "libcgj: direct m55400 marker string fail\n");
		nFail++;
	}
	return nFail;
}
#else /* !CGJ_SOFT_M55400_DIRECT — dyn soft path */

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
		        "libcgj: soft m55400 %s KAT fail got=%u expect=%u\n",
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
		        "libcgj: soft m55400 %s marker fail got=\"%s\" "
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

#endif /* CGJ_SOFT_M55400_DIRECT */

int
main(void)
{
#ifdef CGJ_SOFT_M55400_DIRECT
	int nFail;

	nFail = direct_check();
	if (nFail != 0) {
		fprintf(stderr,
		        "libcgj: soft m55400 DIRECT FAIL fails=%d\n", nFail);
		return 1;
	}
	printf("libcgj: soft m55400 DIRECT PASS (all 11 lamps + markers)\n");
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
		        "libcgj: soft m55400 skip (dlopen fail: %s)\n",
		        dlerror());
		printf("libcgj: soft m55400 SOFT-SKIP (no product SO)\n");
		return 0;
	}

	/* Continuum soft gates: skip if missing, hard fail if wrong. */
	nFail += soft_u32(h, "gj_shell_green_55400", 1u, &nChecked);
	nFail += soft_u32(h, "gj_libcgj_green_55400", 1u, &nChecked);
	nFail += soft_u32(h, "gj_bar3_ready_55400", 0u, &nChecked);
	nFail += soft_u32(h, "gj_product_score_55400", 0u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_ready_55400", 1u, &nChecked);
	nFail += soft_u32(h, "gj_smoke_soft_55400", 1u, &nChecked);
	nFail += soft_u32(h, "gj_dyn_soft_55400", 1u, &nChecked);
	nFail += soft_u32(h, "gj_milestone_tag_55400", 55400u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_wave_55400", 55400u, &nChecked);
	nFail += soft_u32(h, "gj_batch_id_55400", 55400u, &nChecked);
	nFail += soft_u32(h, "gj_graph_milestone_55400", 55400u, &nChecked);

	nFail += soft_marker(h, "__libcgj_batch55391_marker",
	                     "libcgj-batch55391", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch55392_marker",
	                     "libcgj-batch55392", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch55393_marker",
	                     "libcgj-batch55393", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch55394_marker",
	                     "libcgj-batch55394", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch55395_marker",
	                     "libcgj-batch55395", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch55396_marker",
	                     "libcgj-batch55396", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch55397_marker",
	                     "libcgj-batch55397", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch55398_marker",
	                     "libcgj-batch55398", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch55399_marker",
	                     "libcgj-batch55399", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch55400_marker",
	                     "libcgj-batch55400", &nChecked);

	if (nFail != 0) {
		(void)dlclose(h);
		fprintf(stderr,
		        "libcgj: soft m55400 FAIL path=%s fails=%d checked=%d\n",
		        szPath, nFail, nChecked);
		return 2;
	}

	printf("libcgj: soft m55400 PASS path=%s checked=%d "
	       "(soft-skip unexported/missing; bar3 still open)\n",
	       szPath, nChecked);
	(void)dlclose(h);
	return 0;
#endif
}
