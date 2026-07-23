/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1830: XML/HTML tag exclusive milestone.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1830(void);
 *     — Returns the compile-time graph batch number for this TU (1830).
 *       Milestone for the XML/HTML tag exclusive pure-data series
 *       (batches 1821–1829: void-tag hint, name start/char preds,
 *       HTML ws / skip_ws_n, amp-entity start, attr quote, CDATA /
 *       comment open predicates).
 *   uint32_t __gj_batch_id_1830  (alias)
 *   __libcgj_batch1830_marker = "libcgj-batch1830"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1830_marker[] = "libcgj-batch1830";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b1830_id(void)
{
	return 1830u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1830 — report this TU's graph batch number.
 *
 * Always returns 1830. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_1830(void)
{
	return b1830_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_1830(void)
    __attribute__((alias("gj_batch_id_1830")));
