/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1611: round-robin next index (scheduler pure data).
 *
 * Surface (unique symbols):
 *   size_t gj_rr_next_index(size_t cur, size_t n);
 *     — Next slot after cur in a ring of n elements: (cur + 1) % n.
 *       n == 0 → 0. cur >= n is reduced mod n first (stable wrap).
 *   size_t __gj_rr_next_index  (alias)
 *   __libcgj_batch1611_marker = "libcgj-batch1611"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1611_marker[] = "libcgj-batch1611";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rr_next_index — advance a round-robin cursor by one.
 *
 * Returns the next index in [0, n). Empty ring (n == 0) returns 0.
 * If cur is outside [0, n), it is reduced modulo n before advancing so
 * callers with stale cursors still land on a valid successor.
 */
size_t
gj_rr_next_index(size_t uCur, size_t cN)
{
	if (cN == 0u) {
		return 0u;
	}
	if (uCur >= cN) {
		uCur = uCur % cN;
	}
	return (uCur + 1u) % cN;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_rr_next_index(size_t uCur, size_t cN)
    __attribute__((alias("gj_rr_next_index")));
