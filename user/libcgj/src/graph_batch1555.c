/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1555: non-atomic seqlock write-begin hint.
 *
 * Surface (unique symbols):
 *   void gj_seqlock_hint_write_begin(volatile uint32_t *seq);
 *     — Enter a writer critical section by making *seq odd (post-increment).
 *       Classic seqlock: even = idle, odd = writer active.
 *       NULL is a no-op. Plain volatile RMW; NOT atomic / no fence.
 *   void __gj_seqlock_hint_write_begin  (alias)
 *   __libcgj_batch1555_marker = "libcgj-batch1555"
 *
 * Protocol (with batch1556 write_end and batch1557/1558 read side):
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

const char __libcgj_batch1555_marker[] = "libcgj-batch1555";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seqlock_hint_write_begin — bump sequence even→odd (writer enter).
 *
 * seq: pointer to shared sequence word; NULL → no-op.
 *
 * Callers should start with an even sequence (idle). Post-increment
 * makes it odd for the duration of the write critical section.
 */
void
gj_seqlock_hint_write_begin(volatile uint32_t *pSeq)
{
	if (pSeq == NULL) {
		return;
	}
	(*pSeq)++;
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_seqlock_hint_write_begin(volatile uint32_t *pSeq)
    __attribute__((alias("gj_seqlock_hint_write_begin")));
