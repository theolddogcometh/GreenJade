/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1579: test that all bits of a 32-bit mask are set.
 *
 * Surface (unique symbols):
 *   int gj_flags32_test_all(uint32_t flags, uint32_t mask);
 *     — Return 1 if (flags & mask) == mask, else 0. Mask 0 → 1 (vacuous
 *       truth). Pure flag arithmetic; no bit-matrix coupling.
 *   int __gj_flags32_test_all  (alias)
 *   __libcgj_batch1579_marker = "libcgj-batch1579"
 *
 * Distinct from gj_bitset_test (batch118) and gj_flags32_set/clear_mask
 * (batch1577/1578). Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1579_marker[] = "libcgj-batch1579";

/* ---- freestanding helpers ---------------------------------------------- */

/* True iff every bit of u32Mask is set in u32Flags. */
static int
b1579_all_set(uint32_t u32Flags, uint32_t u32Mask)
{
	return ((u32Flags & u32Mask) == u32Mask) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flags32_test_all — check that all mask bits are present in flags.
 *
 * Returns 1 when every bit of u32Mask is set in u32Flags; else 0.
 * u32Mask == 0 always yields 1.
 */
int
gj_flags32_test_all(uint32_t u32Flags, uint32_t u32Mask)
{
	return b1579_all_set(u32Flags, u32Mask);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_flags32_test_all(uint32_t u32Flags, uint32_t u32Mask)
    __attribute__((alias("gj_flags32_test_all")));
