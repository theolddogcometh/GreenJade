/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch548: classic seqlock read side (plain loads).
 *
 * Surface (unique symbols):
 *   int gj_seqlock_read_begin(const uint32_t *seq);
 *     — Load *seq. If even, return it. If odd (writer mid-update), spin-read
 *       until an even value is observed, then return that value. Returns 0
 *       if seq is NULL.
 *   int __gj_seqlock_read_begin  (alias)
 *   int gj_seqlock_read_retry(const uint32_t *seq, uint32_t start);
 *     — After a critical section, re-load *seq. Returns 1 if the sequence
 *       changed from start or is currently odd (writer active / torn read);
 *       returns 0 if still even and equal to start (consistent read).
 *       Returns 1 if seq is NULL.
 *   int __gj_seqlock_read_retry  (alias)
 *   __libcgj_batch548_marker = "libcgj-batch548"
 *
 * Classic seqlock reader protocol (no atomics; plain loads only):
 *   start = gj_seqlock_read_begin(&lock->seq);
 *   ... copy shared data ...
 *   if (gj_seqlock_read_retry(&lock->seq, (uint32_t)start))
 *       retry;
 *
 * Writer side (not in this TU): seq odd while mutating, even when idle.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * atomics, no libc string calls. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch548_marker[] = "libcgj-batch548";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seqlock_read_begin — sample an even sequence number for a seqlock read.
 *
 * seq: pointer to the shared sequence word (writer toggles odd/even).
 *
 * Returns the observed even sequence as int, or 0 if seq is NULL. Spins
 * (plain re-load) while the value is odd so the caller starts outside a
 * writer critical section.
 */
int
gj_seqlock_read_begin(const uint32_t *pSeq)
{
	uint32_t u32Seq;

	if (pSeq == NULL) {
		return 0;
	}

	u32Seq = *pSeq;
	while ((u32Seq & 1u) != 0u) {
		/* Writer mid-update — spin-read until sequence goes even. */
		u32Seq = *pSeq;
	}
	return (int)u32Seq;
}

/*
 * gj_seqlock_read_retry — true if the read must be retried.
 *
 * seq:   same sequence word passed to begin
 * start: value previously returned by gj_seqlock_read_begin (as uint32_t)
 *
 * Returns 1 if *seq != start or *seq is odd (or seq is NULL); 0 if the
 * sequence is still even and matches start (consistent snapshot).
 */
int
gj_seqlock_read_retry(const uint32_t *pSeq, uint32_t u32Start)
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

/* ---- underscored aliases ----------------------------------------------- */

int __gj_seqlock_read_begin(const uint32_t *pSeq)
    __attribute__((alias("gj_seqlock_read_begin")));

int __gj_seqlock_read_retry(const uint32_t *pSeq, uint32_t u32Start)
    __attribute__((alias("gj_seqlock_read_retry")));
