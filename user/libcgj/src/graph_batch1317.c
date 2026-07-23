/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1317: Countdown latch tick (decrement, no underflow).
 *
 * Surface (unique symbols):
 *   uint32_t gj_countdown_tick(uint32_t *remaining);
 *     — If *remaining > 0, decrement by one and return the new value.
 *       NULL or already zero → 0 (no underflow).
 *   uint32_t __gj_countdown_tick  (alias)
 *   __libcgj_batch1317_marker = "libcgj-batch1317"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1317_marker[] = "libcgj-batch1317";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_countdown_tick — one step of a countdown latch.
 *
 * Returns the remaining count after a successful decrement. A return of
 * 0 means the latch is done (or was already done / NULL).
 */
uint32_t
gj_countdown_tick(uint32_t *pRemaining)
{
	uint32_t uCur;

	if (pRemaining == NULL) {
		return 0u;
	}
	uCur = *pRemaining;
	if (uCur == 0u) {
		return 0u;
	}
	uCur--;
	*pRemaining = uCur;
	return uCur;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_countdown_tick(uint32_t *pRemaining)
    __attribute__((alias("gj_countdown_tick")));

