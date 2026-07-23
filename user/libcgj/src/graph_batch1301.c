/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1301: FIFO occupancy for free-running write/read cursors.
 *
 * Surface (unique symbols):
 *   size_t gj_fifo_depth(size_t w, size_t r);
 *     — Occupancy of a free-running SPSC/FIFO ring: (w - r) when
 *       w >= r; else 0 (refuse inverted cursors).
 *   size_t __gj_fifo_depth  (alias)
 *   __libcgj_batch1301_marker = "libcgj-batch1301"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */
#include <stddef.h>

const char __libcgj_batch1301_marker[] = "libcgj-batch1301";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fifo_depth — elements enqueued but not yet dequeued.
 *
 * Free-running producer write index w and consumer read index r
 * (same model as batch883/884). Depth is w - r when ordered; inverted
 * cursors return 0 rather than a wrap-around guess.
 */
size_t
gj_fifo_depth(size_t uW, size_t uR)
{
	if (uW < uR) {
		return 0u;
	}
	return uW - uR;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_fifo_depth(size_t uW, size_t uR)
    __attribute__((alias("gj_fifo_depth")));

