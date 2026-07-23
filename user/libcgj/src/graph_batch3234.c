/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3234: clear bits under a mask in a uint32_t (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_clear_bits_u(uint32_t x, uint32_t mask);
 *     - Return x with every bit set in mask cleared (x & ~mask). Bits
 *       outside the mask are preserved. Pure integer AND-NOT; no
 *       builtins, no shift UB.
 *   uint32_t __gj_u32_clear_bits_u  (alias)
 *   __libcgj_batch3234_marker = "libcgj-batch3234"
 *
 * CREATE-ONLY exclusive bitops wave (3231-3240). Unique
 * gj_u32_clear_bits_u surface only; no multi-def. Distinct from
 * gj_u32_clear_bit (batch2018) and gj_bit_clr_u32 (batch2723) which
 * clear a single indexed bit. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3234_marker[] = "libcgj-batch3234";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3234_clear(uint32_t u32X, uint32_t u32Mask)
{
	return u32X & ~u32Mask;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_clear_bits_u - clear every bit of x that is set in mask.
 *
 * x:    value to update
 * mask: bits to clear (1 = clear that bit in x)
 *
 * Returns x & ~mask. Does not call libc. No parent wires.
 */
uint32_t
gj_u32_clear_bits_u(uint32_t u32X, uint32_t u32Mask)
{
	(void)NULL;
	return b3234_clear(u32X, u32Mask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_clear_bits_u(uint32_t u32X, uint32_t u32Mask)
    __attribute__((alias("gj_u32_clear_bits_u")));
