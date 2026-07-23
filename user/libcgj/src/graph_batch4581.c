/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4581: set bits in a uint32_t flag word (value).
 *
 * Surface (unique symbols):
 *   uint32_t gj_flags_set_u(uint32_t f, uint32_t bits);
 *     - Return f | bits. Pure value transform; does not mutate through
 *       a pointer. bits 0 leaves f unchanged.
 *   uint32_t __gj_flags_set_u  (alias)
 *   __libcgj_batch4581_marker = "libcgj-batch4581"
 *
 * Exclusive continuum CREATE-ONLY (4581-4590: flags_set_u, flags_clear_u,
 * flags_toggle_u, flags_test_all_u, flags_test_any_u, flags_test_none_u,
 * flags_mask_u, flags_diff_u, flags_subset_u, batch_id_4590).
 * Distinct from gj_flags32_set_mask (batch1577, pointer OR-into) —
 * unique gj_flags_set_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4581_marker[] = "libcgj-batch4581";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4581_set(uint32_t u32F, uint32_t u32Bits)
{
	return u32F | u32Bits;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flags_set_u - set every bit of bits in flag word f (value OR).
 *
 * f:    current flag word
 * bits: bits to force on
 *
 * Returns f | bits. Self-contained; no parent wires.
 */
uint32_t
gj_flags_set_u(uint32_t u32F, uint32_t u32Bits)
{
	(void)NULL;
	return b4581_set(u32F, u32Bits);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_flags_set_u(uint32_t u32F, uint32_t u32Bits)
    __attribute__((alias("gj_flags_set_u")));
