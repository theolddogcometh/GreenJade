/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1552: non-atomic spinlock hint try-acquire.
 *
 * Surface (unique symbols):
 *   int gj_spinlock_hint_try(volatile uint32_t *lock);
 *     — If *lock is free (0), set held (1) and return 1. If already
 *       held, return 0 without waiting. NULL lock → 0.
 *       Plain volatile load/store; NOT an atomic CAS.
 *   int __gj_spinlock_hint_try  (alias)
 *   __libcgj_batch1552_marker = "libcgj-batch1552"
 *
 * Convention (shared with batch1551/1553):
 *   *lock == 0 → free
 *   *lock != 0 → held
 *
 * NON-ATOMIC HINT: racy under true concurrency. For cooperative /
 * single-thread ownership transfer only unless externally serialized.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no atomics. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1552_marker[] = "libcgj-batch1552";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_spinlock_hint_try — non-blocking cooperative try-lock.
 *
 * lock: pointer to volatile flag (0 free / nonzero held); NULL → 0.
 *
 * Returns 1 if this call transitioned free→held, else 0.
 */
int
gj_spinlock_hint_try(volatile uint32_t *pLock)
{
	if (pLock == NULL) {
		return 0;
	}
	if (*pLock != 0u) {
		return 0;
	}
	*pLock = 1u;
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_spinlock_hint_try(volatile uint32_t *pLock)
    __attribute__((alias("gj_spinlock_hint_try")));
