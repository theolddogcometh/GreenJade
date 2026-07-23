/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3828: little-endian uint64 wire value to host (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_from_le_u(uint64_t x);
 *     - Interpret x as a little-endian wire word and return the host-order
 *       numerical value. On GreenJade LE x86_64 this is the identity.
 *   uint64_t __gj_u64_from_le_u  (alias)
 *   __libcgj_batch3828_marker = "libcgj-batch3828"
 *
 * Exclusive continuum CREATE-ONLY (3821-3830). Distinct from
 * gj_load_le64 / gj_u64_from_be_u — pure integer from_le_u surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3828_marker[] = "libcgj-batch3828";

/* ---- freestanding helpers ---------------------------------------------- */

/* LE wire → host on LE: identity. */
static uint64_t
b3828_from_le(uint64_t u64X)
{
	return u64X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_from_le_u - convert a little-endian uint64 wire value to host order.
 *
 * x: value whose bit pattern is little-endian wire layout.
 * Returns host-order numerical value (identity on LE hosts).
 */
uint64_t
gj_u64_from_le_u(uint64_t u64X)
{
	(void)NULL;
	return b3828_from_le(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_from_le_u(uint64_t u64X)
    __attribute__((alias("gj_u64_from_le_u")));
