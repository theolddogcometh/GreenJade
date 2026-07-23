/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1302: FIFO free slots for free-running write/read + capacity.
 *
 * Surface (unique symbols):
 *   size_t gj_fifo_space(size_t w, size_t r, size_t cap);
 *     — Free slots: cap - depth when depth <= cap; else 0.
 *       cap == 0 → 0. Inverted cursors (w < r) → 0.
 *   size_t __gj_fifo_space  (alias)
 *   __libcgj_batch1302_marker = "libcgj-batch1302"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */
#include <stddef.h>

const char __libcgj_batch1302_marker[] = "libcgj-batch1302";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fifo_space — remaining free slots in a free-running FIFO/SPSC ring.
 *
 * depth = w - r (when w >= r). space = cap - depth when depth <= cap.
 * Returns 0 if cap is 0, cursors are inverted, or occupancy exceeds cap.
 */
size_t
gj_fifo_space(size_t uW, size_t uR, size_t uCap)
{
	size_t uDepth;

	if (uCap == 0u || uW < uR) {
		return 0u;
	}
	uDepth = uW - uR;
	if (uDepth > uCap) {
		return 0u;
	}
	return uCap - uDepth;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_fifo_space(size_t uW, size_t uR, size_t uCap)
    __attribute__((alias("gj_fifo_space")));

