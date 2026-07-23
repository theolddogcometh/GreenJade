/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3238: toggle bits under a mask in a uint32_t (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_toggle_bits_u(uint32_t x, uint32_t mask);
 *     - Return x with every bit set in mask flipped (x ^ mask). Bits
 *       outside the mask are preserved. Pure integer XOR; no builtins.
 *   uint32_t __gj_u32_toggle_bits_u  (alias)
 *   __libcgj_batch3238_marker = "libcgj-batch3238"
 *
 * CREATE-ONLY exclusive bitops wave (3231-3240). Unique
 * gj_u32_toggle_bits_u surface only; no multi-def. Distinct from
 * gj_bit_flip_u32 (batch2724) which toggles a single indexed bit.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3238_marker[] = "libcgj-batch3238";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3238_toggle(uint32_t u32X, uint32_t u32Mask)
{
	return u32X ^ u32Mask;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_toggle_bits_u - flip every bit of x that is set in mask.
 *
 * x:    value to update
 * mask: bits to toggle (1 = flip that bit in x)
 *
 * Returns x ^ mask. Does not call libc. No parent wires.
 */
uint32_t
gj_u32_toggle_bits_u(uint32_t u32X, uint32_t u32Mask)
{
	(void)NULL;
	return b3238_toggle(u32X, u32Mask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_toggle_bits_u(uint32_t u32X, uint32_t u32Mask)
    __attribute__((alias("gj_u32_toggle_bits_u")));
