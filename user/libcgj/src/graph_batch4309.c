/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4309: set bit i of uint32_t (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_bit_set_u(uint32_t x, unsigned i);
 *     - Return x with bit i set. If i >= 32, return x unchanged
 *       (no shift UB). Pure integer; no builtins.
 *   uint32_t __gj_u32_bit_set_u  (alias)
 *   __libcgj_batch4309_marker = "libcgj-batch4309"
 *
 * Exclusive continuum CREATE-ONLY (4301-4310). Distinct from
 * gj_bit_set_u64 (batch651) and gj_u64_set_bit (batch2011) — unique
 * gj_u32_bit_set_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4309_marker[] = "libcgj-batch4309";

/* ---- freestanding helpers ---------------------------------------------- */

/* Set bit i; i >= 32 leaves x unchanged. */
static uint32_t
b4309_bit_set(uint32_t u32X, unsigned uI)
{
	if (uI >= 32u) {
		return u32X;
	}
	return u32X | (1u << uI);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_bit_set_u - set bit i of a 32-bit word.
 *
 * x: value to update
 * i: bit index; i >= 32 -> x unchanged
 *
 * Returns x with bit i set, or x if i is out of range. Does not call
 * libc. Self-contained; no parent wires.
 */
uint32_t
gj_u32_bit_set_u(uint32_t u32X, unsigned uI)
{
	(void)NULL;
	return b4309_bit_set(u32X, uI);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_bit_set_u(uint32_t u32X, unsigned uI)
    __attribute__((alias("gj_u32_bit_set_u")));
