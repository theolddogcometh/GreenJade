/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6960: mallocate known mallopt flag param ok.
 *
 * Surface (unique symbols):
 *   int gj_mallocate_flag_param_ok_6960(int param);
 *     - Returns 1 if param is a soft-known mallopt flag code
 *       (M_MXFAST=1, M_TRIM_THRESHOLD=-1 .. M_ARENA_MAX=-8), else 0.
 *       Catalog predicate only; does not call mallopt or touch the heap.
 *   int __gj_mallocate_flag_param_ok_6960  (alias)
 *   __libcgj_batch6960_marker = "libcgj-batch6960"
 *
 * Mallocate flags stubs exclusive CREATE-ONLY wave (6951-6960). Unique
 * batch-suffixed surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6960_marker[] = "libcgj-batch6960";

/* Soft mallopt param catalog (glibc-shaped ids). */
#define B6960_M_MXFAST           1
#define B6960_M_TRIM_THRESHOLD  (-1)
#define B6960_M_TOP_PAD         (-2)
#define B6960_M_MMAP_THRESHOLD  (-3)
#define B6960_M_MMAP_MAX        (-4)
#define B6960_M_CHECK_ACTION    (-5)
#define B6960_M_PERTURB         (-6)
#define B6960_M_ARENA_TEST      (-7)
#define B6960_M_ARENA_MAX       (-8)

/* ---- freestanding helpers ---------------------------------------------- */

/* Return 1 if nParam matches a soft-known mallopt flag code. */
static int
b6960_flag_param_ok(int nParam)
{
	if (nParam == B6960_M_MXFAST) {
		return 1;
	}
	if (nParam == B6960_M_TRIM_THRESHOLD) {
		return 1;
	}
	if (nParam == B6960_M_TOP_PAD) {
		return 1;
	}
	if (nParam == B6960_M_MMAP_THRESHOLD) {
		return 1;
	}
	if (nParam == B6960_M_MMAP_MAX) {
		return 1;
	}
	if (nParam == B6960_M_CHECK_ACTION) {
		return 1;
	}
	if (nParam == B6960_M_PERTURB) {
		return 1;
	}
	if (nParam == B6960_M_ARENA_TEST) {
		return 1;
	}
	if (nParam == B6960_M_ARENA_MAX) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mallocate_flag_param_ok_6960 - soft-known mallopt flag param check.
 *
 * param: mallopt-style option code under test.
 *
 * Returns 1 for M_MXFAST (1) and M_TRIM_THRESHOLD..M_ARENA_MAX (-1..-8);
 * otherwise 0. Pure catalog predicate; no heap side effects.
 * No parent wires.
 */
int
gj_mallocate_flag_param_ok_6960(int nParam)
{
	(void)NULL;
	return b6960_flag_param_ok(nParam);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mallocate_flag_param_ok_6960(int nParam)
    __attribute__((alias("gj_mallocate_flag_param_ok_6960")));
