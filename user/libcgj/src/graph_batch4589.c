/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4589: test that flag word a is a subset of b.
 *
 * Surface (unique symbols):
 *   uint32_t gj_flags_subset_u(uint32_t a, uint32_t b);
 *     - Return 1 if (a & b) == a (every bit of a is set in b), else 0.
 *       a 0 → 1 (empty subset). Pure flag arithmetic; value-only.
 *   uint32_t __gj_flags_subset_u  (alias)
 *   __libcgj_batch4589_marker = "libcgj-batch4589"
 *
 * Exclusive continuum CREATE-ONLY (4581-4590). Distinct from
 * gj_flags_test_all_u (batch4584, (f & bits) == bits with f/bits
 * roles) — unique gj_flags_subset_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4589_marker[] = "libcgj-batch4589";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4589_subset(uint32_t u32A, uint32_t u32B)
{
	if ((u32A & u32B) == u32A) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flags_subset_u - 1 iff every bit of a is also set in b.
 *
 * a: candidate subset flag word
 * b: candidate superset flag word
 *
 * Returns 1 when (a & b) == a, else 0. a == 0 always yields 1.
 * No parent wires.
 */
uint32_t
gj_flags_subset_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b4589_subset(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_flags_subset_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_flags_subset_u")));
