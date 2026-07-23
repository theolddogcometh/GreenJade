/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3235: clear bits under a mask in a uint64_t (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_clear_bits_u(uint64_t x, uint64_t mask);
 *     - Return x with every bit set in mask cleared (x & ~mask). Bits
 *       outside the mask are preserved. Pure integer AND-NOT; no
 *       builtins, no shift UB.
 *   uint64_t __gj_u64_clear_bits_u  (alias)
 *   __libcgj_batch3235_marker = "libcgj-batch3235"
 *
 * CREATE-ONLY exclusive bitops wave (3231-3240). Unique
 * gj_u64_clear_bits_u surface only; no multi-def. Distinct from
 * gj_u64_clear_bit (batch2012), gj_bit_clear_u64 (batch652), and
 * gj_u32_clear_bits_u (batch3234). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3235_marker[] = "libcgj-batch3235";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b3235_clear(uint64_t u64X, uint64_t u64Mask)
{
	return u64X & ~u64Mask;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_clear_bits_u - clear every bit of x that is set in mask.
 *
 * x:    value to update
 * mask: bits to clear (1 = clear that bit in x)
 *
 * Returns x & ~mask. Does not call libc. No parent wires.
 */
uint64_t
gj_u64_clear_bits_u(uint64_t u64X, uint64_t u64Mask)
{
	(void)NULL;
	return b3235_clear(u64X, u64Mask);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_clear_bits_u(uint64_t u64X, uint64_t u64Mask)
    __attribute__((alias("gj_u64_clear_bits_u")));
