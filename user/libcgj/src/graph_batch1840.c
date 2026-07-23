/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1840: float bit-pattern exclusive milestone.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1840(void);
 *     — Returns the compile-time graph batch number for this TU (1840).
 *       Milestone for the float bit-pattern exclusive pure-integer series
 *       (batches 1831–1839: f32 is_nan/is_inf/is_zero/sign, f64
 *       is_nan/is_inf/is_zero/sign, f32 abs — all IEEE bit patterns only).
 *   uint32_t __gj_batch_id_1840  (alias)
 *   __libcgj_batch1840_marker = "libcgj-batch1840"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1840_marker[] = "libcgj-batch1840";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b1840_id(void)
{
	return 1840u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1840 — report this TU's graph batch number.
 *
 * Always returns 1840. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_1840(void)
{
	return b1840_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_1840(void)
    __attribute__((alias("gj_batch_id_1840")));
