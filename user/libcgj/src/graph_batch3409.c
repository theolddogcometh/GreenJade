/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3409: u32 BLSMSK (mask up to lowest set bit).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_blsmsk_u(uint32_t x);
 *     - Return x ^ (x - 1): mask from bit 0 through the least-
 *       significant 1-bit of x (all ones when x == 0). Pure unsigned
 *       arithmetic; no builtins.
 *   uint32_t __gj_u32_blsmsk_u  (alias)
 *   __libcgj_batch3409_marker = "libcgj-batch3409"
 *
 * Exclusive continuum CREATE-ONLY (3401-3410). Distinct from
 * gj_u32_blsi_u (batch3405) / gj_u32_blsr_u (batch3407) —
 * unique gj_u32_blsmsk_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3409_marker[] = "libcgj-batch3409";

/* ---- freestanding helpers ---------------------------------------------- */

/* BMI1 BLSMSK: mask through lowest set bit via x ^ (x - 1). */
static uint32_t
b3409_blsmsk(uint32_t uX)
{
	return uX ^ (uX - 1u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_blsmsk_u - mask from bit 0 through the lowest set bit of x.
 *
 * x: unsigned 32-bit value
 *
 * Identity: x ^ (x - 1). Examples: 0 → 0xffffffffu; 0x1 → 0x1;
 * 0x10 → 0x1f; 0x12 → 0x3. No parent wires.
 */
uint32_t
gj_u32_blsmsk_u(uint32_t uX)
{
	(void)NULL;
	return b3409_blsmsk(uX);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_blsmsk_u(uint32_t uX)
    __attribute__((alias("gj_u32_blsmsk_u")));
