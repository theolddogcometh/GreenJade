/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3437: 64-bit adjacent nibble swap (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_nibble_swap_u(uint64_t x);
 *     - Within each byte, exchange high and low 4-bit nibbles
 *       (SWAR over all eight bytes). Pure integer.
 *   uint64_t __gj_u64_nibble_swap_u  (alias)
 *   __libcgj_batch3437_marker = "libcgj-batch3437"
 *
 * CREATE-ONLY exclusive continuum wave (3431-3440). Unique
 * gj_u64_nibble_swap_u surface only; no multi-def. Distinct from
 * gj_nibble_swap (batch938) and gj_u64_nibble_reverse (batch2138).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3437_marker[] = "libcgj-batch3437";

/* ---- freestanding helpers ---------------------------------------------- */

/* Swap adjacent nibbles in every byte of a 64-bit word. */
static uint64_t
b3437_nibswap(uint64_t u64X)
{
	return ((u64X & 0x0f0f0f0f0f0f0f0full) << 4) |
	       ((u64X & 0xf0f0f0f0f0f0f0f0ull) >> 4);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_nibble_swap_u - swap high/low nibbles within each byte.
 *
 * x: value whose per-byte nibble pairs are to be swapped
 *
 * Example: 0x12ab34cd56ef7890 -> 0x21ba43dc65fe8709.
 * Does not call libc. No parent wires.
 */
uint64_t
gj_u64_nibble_swap_u(uint64_t u64X)
{
	(void)NULL;
	return b3437_nibswap(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_nibble_swap_u(uint64_t u64X)
    __attribute__((alias("gj_u64_nibble_swap_u")));
