/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1909: monitor index soft validity.
 *
 * Surface (unique symbols):
 *   int gj_monitor_index_ok(int index);
 *     — Return 1 if index is in the soft multi-monitor range [0, 63]
 *       (primary at 0 plus headroom for multi-head desktops), else 0.
 *   int __gj_monitor_index_ok  (alias)
 *   __libcgj_batch1909_marker = "libcgj-batch1909"
 *
 * Desktop window exclusive pure-data wave (1901–1910). Unique surface
 * only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1909_marker[] = "libcgj-batch1909";

/* Soft closed range for 0-based monitor indices. */
#define B1909_IDX_MIN  0
#define B1909_IDX_MAX  63

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1909_index_ok(int nIndex)
{
	if (nIndex < B1909_IDX_MIN) {
		return 0;
	}
	if (nIndex > B1909_IDX_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_monitor_index_ok — 1 iff index is in [0, 63].
 *
 * index: 0-based monitor / output index (signed so negatives reject).
 * Soft multi-head bound only; not a probe of connected displays.
 */
int
gj_monitor_index_ok(int nIndex)
{
	(void)NULL;
	return b1909_index_ok(nIndex);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_monitor_index_ok(int nIndex)
    __attribute__((alias("gj_monitor_index_ok")));
