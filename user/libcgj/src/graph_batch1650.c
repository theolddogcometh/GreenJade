/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1650: milestone batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1650(void);
 *     — Returns the compile-time graph batch number for this TU (1650).
 *   uint32_t __gj_batch_id_1650  (alias)
 *   __libcgj_batch1650_marker = "libcgj-batch1650"
 *
 * Milestone for the base-conversion exclusive wave (batches 1641–1649).
 * Does NOT redefine gj_batch_id / prior batch_id_* — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1650_marker[] = "libcgj-batch1650";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1650 — report this TU's graph batch number.
 *
 * Always returns 1650. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_1650(void)
{
	return 1650u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_1650(void)
    __attribute__((alias("gj_batch_id_1650")));
