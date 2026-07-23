/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4584: test that all requested flag bits are set.
 *
 * Surface (unique symbols):
 *   uint32_t gj_flags_test_all_u(uint32_t f, uint32_t bits);
 *     - Return 1 if (f & bits) == bits, else 0. bits 0 → 1 (vacuous
 *       truth). Pure flag arithmetic; value-only (no pointer).
 *   uint32_t __gj_flags_test_all_u  (alias)
 *   __libcgj_batch4584_marker = "libcgj-batch4584"
 *
 * Exclusive continuum CREATE-ONLY (4581-4590). Distinct from
 * gj_flags32_test_all (batch1579, int return) — unique
 * gj_flags_test_all_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4584_marker[] = "libcgj-batch4584";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4584_test_all(uint32_t u32F, uint32_t u32Bits)
{
	if ((u32F & u32Bits) == u32Bits) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flags_test_all_u - 1 iff every bit of bits is set in f.
 *
 * f:    flag word under test
 * bits: required bits (mask)
 *
 * Returns 1 when (f & bits) == bits, else 0. bits == 0 always yields 1.
 * No parent wires.
 */
uint32_t
gj_flags_test_all_u(uint32_t u32F, uint32_t u32Bits)
{
	(void)NULL;
	return b4584_test_all(u32F, u32Bits);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_flags_test_all_u(uint32_t u32F, uint32_t u32Bits)
    __attribute__((alias("gj_flags_test_all_u")));
