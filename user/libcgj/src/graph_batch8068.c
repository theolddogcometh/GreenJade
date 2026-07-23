/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8068: memcmp order sign normalize stub.
 *
 * Surface (unique symbols):
 *   int gj_memcmp_order_sign_norm_8068(int32_t r);
 *     - Map any signed memcmp delta to -1 / 0 / 1.
 *   int __gj_memcmp_order_sign_norm_8068  (alias)
 *   __libcgj_batch8068_marker = "libcgj-batch8068"
 *
 * Exclusive continuum CREATE-ONLY (8061-8070: memcmp order stubs —
 * eq_id, lt_id, gt_id, is_eq, is_lt, is_gt, is_ne, sign_norm,
 * u8_asc, continuum + batch_id_8070). Unique
 * gj_memcmp_order_sign_norm_8068 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8068_marker[] = "libcgj-batch8068";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b8068_sign_norm(int32_t i32R)
{
	if (i32R < 0)
		return -1;
	if (i32R > 0)
		return 1;
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_memcmp_order_sign_norm_8068 - normalize raw memcmp delta to -1/0/1.
 *
 * r: signed difference or raw memcmp return
 *
 * Returns -1 if r < 0, 0 if r == 0, 1 if r > 0. Soft trichotomy clamp
 * for order ports; does not call libc memcmp. No parent wires.
 */
int
gj_memcmp_order_sign_norm_8068(int32_t i32R)
{
	(void)NULL;
	return b8068_sign_norm(i32R);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_memcmp_order_sign_norm_8068(int32_t i32R)
    __attribute__((alias("gj_memcmp_order_sign_norm_8068")));
