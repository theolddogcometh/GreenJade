/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1610: geometry-integer milestone marker.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1610(void);
 *     — Returns the compile-time graph batch number for this TU (1610).
 *       Milestone for the int32 geometry exclusive series beyond mat2
 *       (batches 1601–1609: rect/AABB/triangle/orient/dist2).
 *   uint32_t __gj_batch_id_1610  (alias)
 *   __libcgj_batch1610_marker = "libcgj-batch1610"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1610_marker[] = "libcgj-batch1610";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1610 — report this TU's graph batch number.
 *
 * Always returns 1610.
 */
uint32_t
gj_batch_id_1610(void)
{
	return 1610u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_1610(void)
    __attribute__((alias("gj_batch_id_1610")));
