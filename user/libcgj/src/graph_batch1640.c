/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1640: milestone batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1640(void);
 *     — Returns the compile-time graph batch number for this TU (1640).
 *       Milestone for the bitset advanced exclusive series
 *       (batches 1631–1639: popcount/parity/isolate/next/prev/and_not/
 *       or_not/subset).
 *   uint32_t __gj_batch_id_1640  (alias)
 *   __libcgj_batch1640_marker = "libcgj-batch1640"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1640_marker[] = "libcgj-batch1640";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1640 — report this TU's graph batch number.
 *
 * Always returns 1640. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_1640(void)
{
	return 1640u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_1640(void)
    __attribute__((alias("gj_batch_id_1640")));
