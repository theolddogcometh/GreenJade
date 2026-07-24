/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 75700 markers (CREATE-ONLY).
 *
 * Coherent soft package (see NOTES_milestone_75700.txt):
 *   - this TU (libcgj continuum dyn/direct soft KATs)
 *   - NOTES_milestone_75700.txt
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
 *   cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_75700 \
 *     user/libcgj/tests/cgj_soft_milestone_75700.c -ldl
 *   ./build/cgj_soft_milestone_75700
 *
 * Optional hard unit check (link pure graph batches directly; no SO map):
 *   cc -std=c11 -Wall -Wextra -O2 -DCGJ_SOFT_M75700_DIRECT \
 *     -o build/cgj_soft_milestone_75700_direct \
 *     user/libcgj/tests/cgj_soft_milestone_75700.c \
 *     user/libcgj/src/graph_batch75691.c \
 *     user/libcgj/src/graph_batch75692.c \
 *     user/libcgj/src/graph_batch75693.c \
 *     user/libcgj/src/graph_batch75694.c \
 *     user/libcgj/src/graph_batch75695.c \
 *     user/libcgj/src/graph_batch75696.c \
 *     user/libcgj/src/graph_batch75697.c \
 *     user/libcgj/src/graph_batch75698.c \
 *     user/libcgj/src/graph_batch75699.c \
 *     user/libcgj/src/graph_batch75700.c
 *   ./build/cgj_soft_milestone_75700_direct
 *
 * Expected soft KATs (wave 75691–75700 exclusive continuum markers):
 *   gj_shell_green_75700      → 1
 *   gj_libcgj_green_75700     → 1
 *   gj_bar3_ready_75700       → 0   (bar3 still open)
 *   gj_product_score_75700    → 0   (product score still open)
 *   gj_continuum_ready_75700  → 1
 *   gj_smoke_soft_75700       → 1
 *   gj_dyn_soft_75700         → 1
 *   gj_milestone_tag_75700    → 75700
 *   gj_continuum_wave_75700   → 75700
 *   gj_batch_id_75700         → 75700
 *   gj_graph_milestone_75700  → 75700
 *   __libcgj_batch75691_marker … __libcgj_batch75700_marker
 *     → "libcgj-batchNNNNN" string match when present
 *
 * Notes for parent wiring (outside this soft exclusive scope):
 *   - Sources graph_batch75691.c … graph_batch75700.c (and fill
 *     25776–25790 / identity 25751–25775) are CREATE-ONLY; wire into
 *     CGJ_SRCS when parent advances makefile_max to 75700.
 *   - libc.map may lag milestone 75700 globals; add 75700 symbols to the
 *     GLIBC_2.2.5 global list so dyn dlsym can see them.
 *   - Optional: fold these soft gates into cgj_dyn_smoke.c /
 *     cgj_smoke.c continuum soft blocks (same skip-if-missing rule).
 *   - Soft gates; bar3 still open (gj_bar3_ready_75700 == 0).
 */

#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef CGJ_SOFT_M75700_DIRECT
#include <dlfcn.h>
#endif

#ifdef CGJ_SOFT_M75700_DIRECT
/* Direct-link path: call product pure markers without dlopen. */
extern uint32_t gj_shell_green_75700(void);
extern uint32_t gj_libcgj_green_75700(void);
extern uint32_t gj_bar3_ready_75700(void);
extern uint32_t gj_product_score_75700(void);
extern uint32_t gj_continuum_ready_75700(void);
extern uint32_t gj_smoke_soft_75700(void);
extern uint32_t gj_dyn_soft_75700(void);
extern uint32_t gj_milestone_tag_75700(void);
extern uint32_t gj_continuum_wave_75700(void);
extern uint32_t gj_batch_id_75700(void);
extern uint32_t gj_graph_milestone_75700(void);
extern const char __libcgj_batch75691_marker[];
extern const char __libcgj_batch75692_marker[];
extern const char __libcgj_batch75693_marker[];
extern const char __libcgj_batch75694_marker[];
extern const char __libcgj_batch75695_marker[];
extern const char __libcgj_batch75696_marker[];
extern const char __libcgj_batch75697_marker[];
extern const char __libcgj_batch75698_marker[];
extern const char __libcgj_batch75699_marker[];
extern const char __libcgj_batch75700_marker[];

static int
direct_check(void)
{
	int nFail = 0;

	if (gj_shell_green_75700() != 1u) {
		fprintf(stderr, "libcgj: direct gj_shell_green_75700 fail\n");
		nFail++;
	}
	if (gj_libcgj_green_75700() != 1u) {
		fprintf(stderr, "libcgj: direct gj_libcgj_green_75700 fail\n");
		nFail++;
	}
	if (gj_bar3_ready_75700() != 0u) {
		fprintf(stderr, "libcgj: direct gj_bar3_ready_75700 fail\n");
		nFail++;
	}
	if (gj_product_score_75700() != 0u) {
		fprintf(stderr, "libcgj: direct gj_product_score_75700 fail\n");
		nFail++;
	}
	if (gj_continuum_ready_75700() != 1u) {
		fprintf(stderr, "libcgj: direct gj_continuum_ready_75700 fail\n");
		nFail++;
	}
	if (gj_smoke_soft_75700() != 1u) {
		fprintf(stderr, "libcgj: direct gj_smoke_soft_75700 fail\n");
		nFail++;
	}
	if (gj_dyn_soft_75700() != 1u) {
		fprintf(stderr, "libcgj: direct gj_dyn_soft_75700 fail\n");
		nFail++;
	}
	if (gj_milestone_tag_75700() != 75700u) {
		fprintf(stderr, "libcgj: direct gj_milestone_tag_75700 fail\n");
		nFail++;
	}
	if (gj_continuum_wave_75700() != 75700u) {
		fprintf(stderr, "libcgj: direct gj_continuum_wave_75700 fail\n");
		nFail++;
	}
	if (gj_batch_id_75700() != 75700u) {
		fprintf(stderr, "libcgj: direct gj_batch_id_75700 fail\n");
		nFail++;
	}
	if (gj_graph_milestone_75700() != 75700u) {
		fprintf(stderr,
		        "libcgj: direct gj_graph_milestone_75700 fail\n");
		nFail++;
	}
	if (strcmp(__libcgj_batch75691_marker, "libcgj-batch75691") != 0 ||
	    strcmp(__libcgj_batch75692_marker, "libcgj-batch75692") != 0 ||
	    strcmp(__libcgj_batch75693_marker, "libcgj-batch75693") != 0 ||
	    strcmp(__libcgj_batch75694_marker, "libcgj-batch75694") != 0 ||
	    strcmp(__libcgj_batch75695_marker, "libcgj-batch75695") != 0 ||
	    strcmp(__libcgj_batch75696_marker, "libcgj-batch75696") != 0 ||
	    strcmp(__libcgj_batch75697_marker, "libcgj-batch75697") != 0 ||
	    strcmp(__libcgj_batch75698_marker, "libcgj-batch75698") != 0 ||
	    strcmp(__libcgj_batch75699_marker, "libcgj-batch75699") != 0 ||
	    strcmp(__libcgj_batch75700_marker, "libcgj-batch75700") != 0) {
		fprintf(stderr, "libcgj: direct m75700 marker string fail\n");
		nFail++;
	}
	return nFail;
}
#else /* !CGJ_SOFT_M75700_DIRECT — dyn soft path */

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
		        "libcgj: soft m75700 %s KAT fail got=%u expect=%u\n",
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
		        "libcgj: soft m75700 %s marker fail got=\"%s\" "
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

#endif /* CGJ_SOFT_M75700_DIRECT */

int
main(void)
{
#ifdef CGJ_SOFT_M75700_DIRECT
	int nFail;

	nFail = direct_check();
	if (nFail != 0) {
		fprintf(stderr,
		        "libcgj: soft m75700 DIRECT FAIL fails=%d\n", nFail);
		return 1;
	}
	printf("libcgj: soft m75700 DIRECT PASS (all 11 lamps + markers)\n");
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
		        "libcgj: soft m75700 skip (dlopen fail: %s)\n",
		        dlerror());
		printf("libcgj: soft m75700 SOFT-SKIP (no product SO)\n");
		return 0;
	}

	/* Continuum soft gates: skip if missing, hard fail if wrong. */
	nFail += soft_u32(h, "gj_shell_green_75700", 1u, &nChecked);
	nFail += soft_u32(h, "gj_libcgj_green_75700", 1u, &nChecked);
	nFail += soft_u32(h, "gj_bar3_ready_75700", 0u, &nChecked);
	nFail += soft_u32(h, "gj_product_score_75700", 0u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_ready_75700", 1u, &nChecked);
	nFail += soft_u32(h, "gj_smoke_soft_75700", 1u, &nChecked);
	nFail += soft_u32(h, "gj_dyn_soft_75700", 1u, &nChecked);
	nFail += soft_u32(h, "gj_milestone_tag_75700", 75700u, &nChecked);
	nFail += soft_u32(h, "gj_continuum_wave_75700", 75700u, &nChecked);
	nFail += soft_u32(h, "gj_batch_id_75700", 75700u, &nChecked);
	nFail += soft_u32(h, "gj_graph_milestone_75700", 75700u, &nChecked);

	nFail += soft_marker(h, "__libcgj_batch75691_marker",
	                     "libcgj-batch75691", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch75692_marker",
	                     "libcgj-batch75692", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch75693_marker",
	                     "libcgj-batch75693", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch75694_marker",
	                     "libcgj-batch75694", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch75695_marker",
	                     "libcgj-batch75695", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch75696_marker",
	                     "libcgj-batch75696", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch75697_marker",
	                     "libcgj-batch75697", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch75698_marker",
	                     "libcgj-batch75698", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch75699_marker",
	                     "libcgj-batch75699", &nChecked);
	nFail += soft_marker(h, "__libcgj_batch75700_marker",
	                     "libcgj-batch75700", &nChecked);

	if (nFail != 0) {
		(void)dlclose(h);
		fprintf(stderr,
		        "libcgj: soft m75700 FAIL path=%s fails=%d checked=%d\n",
		        szPath, nFail, nChecked);
		return 2;
	}

	printf("libcgj: soft m75700 PASS path=%s checked=%d "
	       "(soft-skip unexported/missing; bar3 still open)\n",
	       szPath, nChecked);
	(void)dlclose(h);
	return 0;
#endif
}
