/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1557: non-atomic seqlock read-begin hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_seqlock_hint_read_begin(const volatile uint32_t *seq);
 *     — Load *seq. If even, return it. If odd (writer mid-update),
 *       spin-read (plain volatile re-load) until even, then return.
 *       NULL seq → 0.
 *   uint32_t __gj_seqlock_hint_read_begin  (alias)
 *   __libcgj_batch1557_marker = "libcgj-batch1557"
 *
 * Classic reader protocol (with batch1558 retry):
 *   start = gj_seqlock_hint_read_begin(&seq);
 *   ... copy shared data ...
 *   if (gj_seqlock_hint_read_retry(&seq, start))
 *       retry;
 *
 * Distinct from gj_seqlock_read_begin (batch548) — hint/volatile surface.
 * Writer side: batch1555/1556.
 *
 * NON-ATOMIC HINT: plain volatile loads only; no fence. Spin is
 * cooperative (writer must eventually restore even sequence).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no atomics. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1557_marker[] = "libcgj-batch1557";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seqlock_hint_read_begin — sample an even sequence for a read section.
 *
 * seq: pointer to shared sequence word; NULL → 0.
 *
 * Returns the observed even sequence. Spins while the value is odd so
 * the caller starts outside a writer critical section.
 */
uint32_t
gj_seqlock_hint_read_begin(const volatile uint32_t *pSeq)
{
	uint32_t u32Seq;

	if (pSeq == NULL) {
		return 0u;
	}

	u32Seq = *pSeq;
	while ((u32Seq & 1u) != 0u) {
		/* Writer mid-update — spin-read until sequence goes even. */
		u32Seq = *pSeq;
	}
	return u32Seq;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_seqlock_hint_read_begin(const volatile uint32_t *pSeq)
    __attribute__((alias("gj_seqlock_hint_read_begin")));
