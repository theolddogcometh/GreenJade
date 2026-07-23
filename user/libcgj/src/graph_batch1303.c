/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1303: LIFO/stack depth accessor (caller-owned top/count).
 *
 * Surface (unique symbols):
 *   size_t gj_lifo_depth(const size_t *top);
 *     — Return *top (stack occupancy / next free slot count).
 *       NULL → 0.
 *   size_t __gj_lifo_depth  (alias)
 *   __libcgj_batch1303_marker = "libcgj-batch1303"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */
#include <stddef.h>

const char __libcgj_batch1303_marker[] = "libcgj-batch1303";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lifo_depth — report LIFO stack depth from caller-owned counter.
 *
 * Convention: *top is the number of live elements (push increments,
 * pop decrements). NULL pointer → 0.
 */
size_t
gj_lifo_depth(const size_t *pTop)
{
	if (pTop == NULL) {
		return 0u;
	}
	return *pTop;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_lifo_depth(const size_t *pTop)
    __attribute__((alias("gj_lifo_depth")));

