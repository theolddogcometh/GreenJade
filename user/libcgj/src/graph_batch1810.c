/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1810: milestone 1810 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1810(void);
 *     — Returns the compile-time graph batch number for this TU (1810).
 *       Milestone for the ring buffer advanced exclusive series
 *       (batches 1801–1809: push/pop/peek/count/space/clear/
 *       full_p/empty_p/discard_n).
 *   uint32_t __gj_batch_id_1810  (alias)
 *   __libcgj_batch1810_marker = "libcgj-batch1810"
 *
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1810_marker[] = "libcgj-batch1810";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1810 — report this TU's graph batch number.
 *
 * Always returns 1810. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_1810(void)
{
	return 1810u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_1810(void)
    __attribute__((alias("gj_batch_id_1810")));
