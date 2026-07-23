/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch330: simple epoch counter grace period stub.
 *
 * Surface (unique symbols):
 *   void gj_epoch_enter(uint64_t *local, uint64_t global);
 *     — Pin *local to the current global epoch (enter a read-side
 *       critical section). Idle threads use local == 0. NULL local is
 *       a no-op.
 *   void gj_epoch_exit(uint64_t *local);
 *     — Leave the critical section: *local = 0 (idle). NULL is a no-op.
 *   int gj_epoch_idle(const uint64_t *locals, size_t n, uint64_t global);
 *     — Grace-period check: return 1 if every slot is idle (0) or already
 *       on global (no participant still pinned to an older epoch).
 *       Return 0 if any locals[i] is non-zero and != global.
 *       Return -1 if locals is NULL when n > 0. n == 0 is idle (1).
 *   __gj_epoch_enter / __gj_epoch_exit / __gj_epoch_idle  (aliases)
 *   __libcgj_batch330_marker = "libcgj-batch330"
 *
 * Model (minimal EBR-style counter, non-atomic single-thread stub):
 *   - Writer advances a shared global epoch after reclaim prep.
 *   - Readers call enter/exit around use of retired objects.
 *   - Reclaim of objects retired before `global` is safe only when
 *     gj_epoch_idle(...) returns 1 (all pins clear or current).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * atomics. Not a full multi-thread EBR; call sites that need concurrent
 * safety must pair with external barriers/atomics. No third-party source
 * copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch330_marker[] = "libcgj-batch330";

/* Idle sentinel: not inside a critical section. */
#define B330_IDLE 0ull

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_epoch_enter — record global epoch into the caller's local slot.
 */
void
gj_epoch_enter(uint64_t *local, uint64_t global)
{
	if (local == NULL) {
		return;
	}
	*local = global;
}

/*
 * gj_epoch_exit — clear the local slot (idle).
 */
void
gj_epoch_exit(uint64_t *local)
{
	if (local == NULL) {
		return;
	}
	*local = B330_IDLE;
}

/*
 * gj_epoch_idle — true if no local pin remains on an epoch other than global.
 *
 * Returns:
 *   1  grace period open: all slots idle or equal to global
 *   0  someone still pinned to an older (or foreign) epoch
 *  -1  invalid args (locals NULL with n > 0)
 */
int
gj_epoch_idle(const uint64_t *locals, size_t n, uint64_t global)
{
	size_t iSlot;

	if (n == 0u) {
		return 1;
	}
	if (locals == NULL) {
		return -1;
	}

	for (iSlot = 0u; iSlot < n; iSlot++) {
		uint64_t u64Local = locals[iSlot];

		if (u64Local != B330_IDLE && u64Local != global) {
			return 0;
		}
	}
	return 1;
}

void __gj_epoch_enter(uint64_t *local, uint64_t global)
    __attribute__((alias("gj_epoch_enter")));
void __gj_epoch_exit(uint64_t *local)
    __attribute__((alias("gj_epoch_exit")));
int __gj_epoch_idle(const uint64_t *locals, size_t n, uint64_t global)
    __attribute__((alias("gj_epoch_idle")));
