/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1556: non-atomic seqlock write-end hint.
 *
 * Surface (unique symbols):
 *   void gj_seqlock_hint_write_end(volatile uint32_t *seq);
 *     — Leave a writer critical section by making *seq even (post-increment).
 *       Pair with gj_seqlock_hint_write_begin (batch1555).
 *       NULL is a no-op. Plain volatile RMW; NOT atomic / no fence.
 *   void __gj_seqlock_hint_write_end  (alias)
 *   __libcgj_batch1556_marker = "libcgj-batch1556"
 *
 * Protocol:
 *   gj_seqlock_hint_write_begin(&seq);  // even → odd
 *   ... mutate shared data ...
 *   gj_seqlock_hint_write_end(&seq);    // odd → even
 *
 * Distinct from gj_seqlock_read_begin/retry (batch548). Unique surface.
 *
 * NON-ATOMIC HINT: cooperative / single-writer harness unless externally
 * serialized. No compiler or hardware memory barrier.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no atomics. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1556_marker[] = "libcgj-batch1556";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seqlock_hint_write_end — bump sequence odd→even (writer leave).
 *
 * seq: pointer to shared sequence word; NULL → no-op.
 *
 * After write_begin the sequence is odd; post-increment restores even
 * so readers can accept a snapshot.
 */
void
gj_seqlock_hint_write_end(volatile uint32_t *pSeq)
{
	if (pSeq == NULL) {
		return;
	}
	(*pSeq)++;
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_seqlock_hint_write_end(volatile uint32_t *pSeq)
    __attribute__((alias("gj_seqlock_hint_write_end")));
