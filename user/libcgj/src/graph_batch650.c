/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch650: milestone batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_650(void);
 *     — Returns the compile-time graph batch number for this TU (650).
 *   uint32_t __gj_batch_id_650  (alias)
 *   __libcgj_batch650_marker = "libcgj-batch650"
 *
 * Does NOT define gj_batch_id (batch500), gj_graph_milestone /
 * gj_batch_id_550 (batch550), gj_batch_id_600 / gj_graph_milestone_600
 * (batch600), or gj_batch_id_630 (batch630) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch650_marker[] = "libcgj-batch650";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_650 — report this TU's graph batch number.
 *
 * Always returns 650. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_650(void)
{
	return 650u;
}

uint32_t __gj_batch_id_650(void)
    __attribute__((alias("gj_batch_id_650")));
