/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1558: non-atomic seqlock read-retry hint.
 *
 * Surface (unique symbols):
 *   int gj_seqlock_hint_read_retry(const volatile uint32_t *seq,
 *                                  uint32_t start);
 *     — After a read critical section, re-load *seq. Returns 1 if the
 *       sequence changed from start or is currently odd (writer active /
 *       torn read); returns 0 if still even and equal to start.
 *       NULL seq → 1 (force retry / fail-closed).
 *   int __gj_seqlock_hint_read_retry  (alias)
 *   __libcgj_batch1558_marker = "libcgj-batch1558"
 *
 * Pair with gj_seqlock_hint_read_begin (batch1557). Distinct from
 * gj_seqlock_read_retry (batch548).
 *
 * NON-ATOMIC HINT: plain volatile load only; no fence.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no atomics. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1558_marker[] = "libcgj-batch1558";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seqlock_hint_read_retry — 1 if the read snapshot must be retried.
 *
 * seq:   same sequence word passed to read_begin
 * start: value previously returned by gj_seqlock_hint_read_begin
 *
 * Returns 1 if *seq != start or *seq is odd (or seq is NULL); 0 if the
 * sequence is still even and matches start (consistent snapshot).
 */
int
gj_seqlock_hint_read_retry(const volatile uint32_t *pSeq, uint32_t u32Start)
{
	uint32_t u32Seq;

	if (pSeq == NULL) {
		return 1;
	}

	u32Seq = *pSeq;
	if ((u32Seq & 1u) != 0u || u32Seq != u32Start) {
		return 1;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_seqlock_hint_read_retry(const volatile uint32_t *pSeq,
    uint32_t u32Start)
    __attribute__((alias("gj_seqlock_hint_read_retry")));
