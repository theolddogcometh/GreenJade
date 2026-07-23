/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4585: test that any requested flag bit is set.
 *
 * Surface (unique symbols):
 *   uint32_t gj_flags_test_any_u(uint32_t f, uint32_t bits);
 *     - Return 1 if (f & bits) != 0, else 0. bits 0 → 0. Pure flag
 *       arithmetic; value-only (no pointer).
 *   uint32_t __gj_flags_test_any_u  (alias)
 *   __libcgj_batch4585_marker = "libcgj-batch4585"
 *
 * Exclusive continuum CREATE-ONLY (4581-4590). Distinct from
 * gj_flags_test_all_u (batch4584) and gj_bitset_any family —
 * unique gj_flags_test_any_u surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4585_marker[] = "libcgj-batch4585";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4585_test_any(uint32_t u32F, uint32_t u32Bits)
{
	if ((u32F & u32Bits) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flags_test_any_u - 1 iff at least one bit of bits is set in f.
 *
 * f:    flag word under test
 * bits: candidate bits (mask)
 *
 * Returns 1 when (f & bits) != 0, else 0. bits == 0 always yields 0.
 * No parent wires.
 */
uint32_t
gj_flags_test_any_u(uint32_t u32F, uint32_t u32Bits)
{
	(void)NULL;
	return b4585_test_any(u32F, u32Bits);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_flags_test_any_u(uint32_t u32F, uint32_t u32Bits)
    __attribute__((alias("gj_flags_test_any_u")));
