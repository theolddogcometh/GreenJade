/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1554: non-atomic rwlock hint init.
 *
 * Surface (unique symbols):
 *   void gj_rwlock_hint_init(volatile uint32_t *state);
 *     — Initialize a cooperative rwlock state word to unlocked (0).
 *       NULL is a no-op. Plain volatile store; NOT an atomic/SMP lock.
 *   void __gj_rwlock_hint_init  (alias)
 *   __libcgj_batch1554_marker = "libcgj-batch1554"
 *
 * Suggested encoding for later try/unlock surfaces (not in this TU):
 *   bit 0       — writer held (1) / free (0)
 *   bits 1..31  — reader count
 *   *state == 0 — fully free
 *
 * NON-ATOMIC HINT: cooperative / single-thread harness only unless
 * externally serialized.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no atomics. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1554_marker[] = "libcgj-batch1554";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rwlock_hint_init — set rwlock state word to unlocked (0).
 *
 * state: pointer to caller-owned volatile word; NULL → no-op.
 */
void
gj_rwlock_hint_init(volatile uint32_t *pState)
{
	if (pState == NULL) {
		return;
	}
	*pState = 0u;
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_rwlock_hint_init(volatile uint32_t *pState)
    __attribute__((alias("gj_rwlock_hint_init")));
