/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1551: non-atomic spinlock hint init.
 *
 * Surface (unique symbols):
 *   void gj_spinlock_hint_init(volatile uint32_t *lock);
 *     — Initialize a cooperative spinlock flag to unlocked (0).
 *       NULL is a no-op. Plain volatile store; NOT an atomic/SMP lock.
 *   void __gj_spinlock_hint_init  (alias)
 *   __libcgj_batch1551_marker = "libcgj-batch1551"
 *
 * Convention (shared with batch1552/1553):
 *   *lock == 0 → free
 *   *lock != 0 → held
 *
 * NON-ATOMIC HINT: for single-thread / cooperative harnesses, or as a
 * sketch of lock state under an external serializer. Not a real mutex.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no atomics. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1551_marker[] = "libcgj-batch1551";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_spinlock_hint_init — set lock word to unlocked (0).
 *
 * lock: pointer to caller-owned volatile flag; NULL → no-op.
 */
void
gj_spinlock_hint_init(volatile uint32_t *pLock)
{
	if (pLock == NULL) {
		return;
	}
	*pLock = 0u;
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_spinlock_hint_init(volatile uint32_t *pLock)
    __attribute__((alias("gj_spinlock_hint_init")));
