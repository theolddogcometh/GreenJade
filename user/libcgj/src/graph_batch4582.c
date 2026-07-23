/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4582: clear bits in a uint32_t flag word (value).
 *
 * Surface (unique symbols):
 *   uint32_t gj_flags_clear_u(uint32_t f, uint32_t bits);
 *     - Return f & ~bits. Pure value transform; does not mutate through
 *       a pointer. bits 0 leaves f unchanged.
 *   uint32_t __gj_flags_clear_u  (alias)
 *   __libcgj_batch4582_marker = "libcgj-batch4582"
 *
 * Exclusive continuum CREATE-ONLY (4581-4590). Distinct from
 * gj_flags32_clear_mask (batch1578, pointer AND-not-into) —
 * unique gj_flags_clear_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4582_marker[] = "libcgj-batch4582";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4582_clear(uint32_t u32F, uint32_t u32Bits)
{
	return u32F & ~u32Bits;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flags_clear_u - clear every bit of bits in flag word f (value AND-not).
 *
 * f:    current flag word
 * bits: bits to force off
 *
 * Returns f & ~bits. Self-contained; no parent wires.
 */
uint32_t
gj_flags_clear_u(uint32_t u32F, uint32_t u32Bits)
{
	(void)NULL;
	return b4582_clear(u32F, u32Bits);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_flags_clear_u(uint32_t u32F, uint32_t u32Bits)
    __attribute__((alias("gj_flags_clear_u")));
