/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4308: test bit i of uint64_t (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u64_bit_test_u(uint64_t x, unsigned i);
 *     - Return 1 if bit i of x is set, else 0. If i >= 64, return 0
 *       (no shift UB). Pure integer; no builtins.
 *   uint32_t __gj_u64_bit_test_u  (alias)
 *   __libcgj_batch4308_marker = "libcgj-batch4308"
 *
 * Exclusive continuum CREATE-ONLY (4301-4310). Distinct from
 * gj_u32_bit_test_u (batch4307), gj_bit_test_u64 (batch653), and
 * gj_u64_test_bit (batch2014) — unique gj_u64_bit_test_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4308_marker[] = "libcgj-batch4308";

/* ---- freestanding helpers ---------------------------------------------- */

/* Test bit i; i >= 64 -> 0. */
static uint32_t
b4308_bit_test(uint64_t u64X, unsigned uI)
{
	if (uI >= 64u) {
		return 0u;
	}
	return (uint32_t)((u64X >> uI) & 1ULL);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_bit_test_u - test whether bit i of a 64-bit word is set.
 *
 * x: value to probe
 * i: bit index; i >= 64 -> 0
 *
 * Returns 1 if bit i is set, else 0. Does not call libc.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u64_bit_test_u(uint64_t u64X, unsigned uI)
{
	(void)NULL;
	return b4308_bit_test(u64X, uI);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u64_bit_test_u(uint64_t u64X, unsigned uI)
    __attribute__((alias("gj_u64_bit_test_u")));
