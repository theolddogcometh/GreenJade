/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2092: freestanding id closed-range predicate.
 *
 * Surface (unique symbols):
 *   int gj_id_is_valid_range(uint64_t id, uint64_t lo, uint64_t hi);
 *     - Return 1 if id is in the inclusive closed range [lo, hi], else 0.
 *       If lo > hi the range is empty and the result is always 0.
 *   int __gj_id_is_valid_range  (alias)
 *   __libcgj_batch2092_marker = "libcgj-batch2092"
 *
 * Post-2000 id exclusive wave (2091-2100). Distinct from
 * gj_id_is_nonzero_p (batch2091) - unique surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2092_marker[] = "libcgj-batch2092";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2092_in_range(uint64_t u64Id, uint64_t u64Lo, uint64_t u64Hi)
{
	if (u64Lo > u64Hi) {
		return 0;
	}
	if (u64Id < u64Lo) {
		return 0;
	}
	if (u64Id > u64Hi) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_id_is_valid_range - true if id lies in inclusive [lo, hi].
 *
 * id: candidate identifier
 * lo: inclusive lower bound
 * hi: inclusive upper bound (lo > hi yields empty range and 0)
 *
 * Returns 1 on membership, else 0. Does not call libc.
 */
int
gj_id_is_valid_range(uint64_t u64Id, uint64_t u64Lo, uint64_t u64Hi)
{
	(void)NULL;
	return b2092_in_range(u64Id, u64Lo, u64Hi);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_id_is_valid_range(uint64_t u64Id, uint64_t u64Lo, uint64_t u64Hi)
    __attribute__((alias("gj_id_is_valid_range")));
