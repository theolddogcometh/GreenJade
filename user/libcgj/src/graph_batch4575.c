/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4575: compare two packed u32 semver words.
 *
 * Surface (unique symbols):
 *   int32_t gj_ver_cmp_u(uint32_t a, uint32_t b);
 *     - Three-way compare of packed semvers a and b. Returns -1 when
 *       a < b, 0 when a == b, and 1 when a > b. With the exclusive
 *       major[23:16]|minor[15:8]|patch[7:0] layout, unsigned numeric
 *       order matches semver lexicographic order for in-range fields.
 *   int32_t __gj_ver_cmp_u  (alias)
 *   __libcgj_batch4575_marker = "libcgj-batch4575"
 *
 * Exclusive continuum CREATE-ONLY (4571-4580). Unique gj_ver_cmp_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4575_marker[] = "libcgj-batch4575";

/* ---- freestanding helpers ---------------------------------------------- */

static int32_t
b4575_cmp(uint32_t u32A, uint32_t u32B)
{
	if (u32A < u32B) {
		return (int32_t)-1;
	}
	if (u32A > u32B) {
		return (int32_t)1;
	}
	return (int32_t)0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ver_cmp_u - three-way compare of packed u32 semver words.
 *
 * a: left-hand packed version
 * b: right-hand packed version
 *
 * Returns -1 if a < b, 0 if a == b, 1 if a > b. Example:
 * gj_ver_cmp_u(0x00010000u, 0x00020000u) -> -1. No parent wires.
 */
int32_t
gj_ver_cmp_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b4575_cmp(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_ver_cmp_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_ver_cmp_u")));
