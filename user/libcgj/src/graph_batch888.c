/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch888: sequence-counter begin (writer side).
 *
 * Classic seqlock writer half: even seq means "stable", odd means
 * "write in progress". begin bumps an even counter to odd.
 *
 * Surface (unique symbols):
 *   uint32_t gj_seqcnt_begin(uint32_t *seq);
 *     — If *seq is even, increment to odd and return the prior even value.
 *       If already odd, leave as-is and return *seq (nested/corrupt).
 *       NULL → return 0.
 *   uint32_t __gj_seqcnt_begin  (alias)
 *   __libcgj_batch888_marker = "libcgj-batch888"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch888_marker[] = "libcgj-batch888";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seqcnt_begin — enter a write section (even → odd).
 *
 * Returns the sequence observed before the bump (even on the normal path)
 * so a paired reader protocol can detect torn reads. Plain sequential —
 * no atomics / barriers (cooperative use).
 */
uint32_t
gj_seqcnt_begin(uint32_t *seq)
{
	uint32_t s;

	if (seq == NULL) {
		return 0u;
	}

	s = *seq;
	/* Even seq: start write by making it odd. */
	if ((s & 1u) == 0u) {
		*seq = s + 1u;
	}
	return s;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_seqcnt_begin(uint32_t *seq)
    __attribute__((alias("gj_seqcnt_begin")));
