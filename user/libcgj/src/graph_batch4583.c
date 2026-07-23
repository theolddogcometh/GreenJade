/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4583: toggle bits in a uint32_t flag word (value).
 *
 * Surface (unique symbols):
 *   uint32_t gj_flags_toggle_u(uint32_t f, uint32_t bits);
 *     - Return f ^ bits. Pure value transform; does not mutate through
 *       a pointer. bits 0 leaves f unchanged.
 *   uint32_t __gj_flags_toggle_u  (alias)
 *   __libcgj_batch4583_marker = "libcgj-batch4583"
 *
 * Exclusive continuum CREATE-ONLY (4581-4590). Distinct from
 * gj_u32_toggle_msb_u (batch4009, single MSB flip) and gj_u64_toggle_bit
 * (batch2013) — unique gj_flags_toggle_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4583_marker[] = "libcgj-batch4583";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4583_toggle(uint32_t u32F, uint32_t u32Bits)
{
	return u32F ^ u32Bits;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flags_toggle_u - flip every bit of bits in flag word f (value XOR).
 *
 * f:    current flag word
 * bits: bits to invert
 *
 * Returns f ^ bits. Self-contained; no parent wires.
 */
uint32_t
gj_flags_toggle_u(uint32_t u32F, uint32_t u32Bits)
{
	(void)NULL;
	return b4583_toggle(u32F, u32Bits);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_flags_toggle_u(uint32_t u32F, uint32_t u32Bits)
    __attribute__((alias("gj_flags_toggle_u")));
