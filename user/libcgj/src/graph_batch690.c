/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch690: milestone batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_690(void);
 *     — Returns the compile-time graph batch number for this TU (690).
 *   uint32_t __gj_batch_id_690  (alias)
 *   __libcgj_batch690_marker = "libcgj-batch690"
 *
 * Does NOT define gj_batch_id (batch500), gj_graph_milestone /
 * gj_batch_id_550 (batch550), gj_batch_id_600 / gj_graph_milestone_600
 * (batch600), gj_batch_id_630 (batch630), gj_batch_id_650 (batch650),
 * or gj_batch_id_670 (batch670) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch690_marker[] = "libcgj-batch690";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_690 — report this TU's graph batch number.
 *
 * Always returns 690. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_690(void)
{
	return 690u;
}

uint32_t __gj_batch_id_690(void)
    __attribute__((alias("gj_batch_id_690")));
