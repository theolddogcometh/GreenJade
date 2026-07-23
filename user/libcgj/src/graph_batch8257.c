/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8257: clear-lowest-set-bit integer stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bit_clear_lsb_u_8257(uint32_t v);
 *     - Return v & (v - 1): clear the least-significant 1-bit of v.
 *       Zero input yields zero.
 *   uint32_t __gj_bit_clear_lsb_u_8257  (alias)
 *   __libcgj_batch8257_marker = "libcgj-batch8257"
 *
 * Exclusive continuum CREATE-ONLY (8251-8260: bitops / ffs-style integer
 * stubs — ffs, clz, ctz, popcount, parity, isolate_lsb, clear_lsb, mask,
 * test, batch_id_8260). Unique gj_bit_clear_lsb_u_8257 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8257_marker[] = "libcgj-batch8257";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b8257_clear_lsb — clear least-significant 1-bit via v & (v - 1).
 */
static uint32_t
b8257_clear_lsb(uint32_t u32V)
{
	return u32V & (u32V - 1u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bit_clear_lsb_u_8257 - clear lowest set bit of a 32-bit word.
 *
 * v: value to update
 *
 * Returns v with its least-significant 1-bit cleared (Brian Kernighan
 * step). Soft clear-lsb catalog surface; pure integer. No parent wires.
 */
uint32_t
gj_bit_clear_lsb_u_8257(uint32_t u32V)
{
	(void)NULL;
	return b8257_clear_lsb(u32V);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bit_clear_lsb_u_8257(uint32_t u32V)
    __attribute__((alias("gj_bit_clear_lsb_u_8257")));
