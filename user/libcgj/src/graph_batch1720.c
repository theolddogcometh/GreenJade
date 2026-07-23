/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1720: milestone batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1720(void);
 *     — Returns the compile-time graph batch number for this TU (1720).
 *       Milestone for the interval / range exclusive series
 *       (batches 1711–1719: u64/i64 contains/overlap/intersect/merge,
 *       u64 clamp/width/mid).
 *   uint32_t __gj_batch_id_1720  (alias)
 *   __libcgj_batch1720_marker = "libcgj-batch1720"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1720_marker[] = "libcgj-batch1720";

/* ---- freestanding helpers ---------------------------------------------- */

/* Compile-time batch number for this TU. */
static uint32_t
b1720_id(void)
{
	return 1720u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1720 — report this TU's graph batch number.
 *
 * Always returns 1720. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_1720(void)
{
	return b1720_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_1720(void)
    __attribute__((alias("gj_batch_id_1720")));
