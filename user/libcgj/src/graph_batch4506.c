/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4506: simple odd-bit swizzle (even/odd swap).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_swizzle_1357_u(uint32_t x);
 *     - Swap each even bit with its neighboring odd bit (positions
 *       0<->1, 2<->3, ...). The name references the odd lane 1,3,5,7
 *       pattern after a single-step interleave. Pure integer SWAR.
 *   uint32_t __gj_u32_swizzle_1357_u  (alias)
 *   __libcgj_batch4506_marker = "libcgj-batch4506"
 *
 * Exclusive continuum CREATE-ONLY (4501-4510). Distinct from
 * gj_u32_rev_bits_u (batch4501) and gj_bitrev32 (batch181) — unique
 * gj_u32_swizzle_1357_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4506_marker[] = "libcgj-batch4506";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4506_swizzle_1357(uint32_t u32X)
{
	/* Even bits -> +1, odd bits -> -1 (simple 1/3/5/7 lane swap). */
	return ((u32X & 0x55555555u) << 1) | ((u32X & 0xAAAAAAAAu) >> 1);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_swizzle_1357_u - interleave-style even/odd bit swap.
 *
 * x: value to swizzle
 *
 * Returns x with each adjacent even/odd bit pair exchanged. Example:
 * gj_u32_swizzle_1357_u(0x1u) -> 0x2u; gj_u32_swizzle_1357_u(0x2u) -> 0x1u.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_swizzle_1357_u(uint32_t u32X)
{
	(void)NULL;
	return b4506_swizzle_1357(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_swizzle_1357_u(uint32_t u32X)
    __attribute__((alias("gj_u32_swizzle_1357_u")));
