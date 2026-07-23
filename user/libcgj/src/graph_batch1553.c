/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1553: non-atomic spinlock hint unlock.
 *
 * Surface (unique symbols):
 *   void gj_spinlock_hint_unlock(volatile uint32_t *lock);
 *     — Clear the lock flag to free (0). NULL is a no-op.
 *       Plain volatile store; NOT a real release barrier.
 *   void __gj_spinlock_hint_unlock  (alias)
 *   __libcgj_batch1553_marker = "libcgj-batch1553"
 *
 * Convention (shared with batch1551/1552):
 *   *lock == 0 → free
 *   *lock != 0 → held
 *
 * NON-ATOMIC HINT: does not validate ownership. Cooperative use only.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no atomics. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1553_marker[] = "libcgj-batch1553";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_spinlock_hint_unlock — set lock word to free (0).
 *
 * lock: pointer to caller-owned volatile flag; NULL → no-op.
 */
void
gj_spinlock_hint_unlock(volatile uint32_t *pLock)
{
	if (pLock == NULL) {
		return;
	}
	*pLock = 0u;
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_spinlock_hint_unlock(volatile uint32_t *pLock)
    __attribute__((alias("gj_spinlock_hint_unlock")));
