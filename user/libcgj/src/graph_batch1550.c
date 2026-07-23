/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1550: milestone batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1550(void);
 *     — Returns the compile-time graph batch number for this TU (1550).
 *   uint32_t __gj_batch_id_1550  (alias)
 *   __libcgj_batch1550_marker = "libcgj-batch1550"
 *
 * Does NOT define gj_batch_id (batch500), gj_batch_id_550 (batch550),
 * gj_batch_id_600 / gj_graph_milestone_600 (batch600), gj_batch_id_630
 * (batch630), or gj_batch_id_650 (batch650) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1550_marker[] = "libcgj-batch1550";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1550 — report this TU's graph batch number.
 *
 * Always returns 1550. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_1550(void)
{
	return 1550u;
}

uint32_t __gj_batch_id_1550(void)
    __attribute__((alias("gj_batch_id_1550")));
