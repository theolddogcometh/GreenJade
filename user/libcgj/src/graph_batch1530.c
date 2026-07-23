/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1530: batch identity milestone marker.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1530(void);
 *     — Returns the compile-time graph batch number for this TU (1530).
 *   uint32_t __gj_batch_id_1530  (alias)
 *   __libcgj_batch1530_marker = "libcgj-batch1530"
 *
 * Milestone for the 1521–1530 freestanding arena/slab/bitmap/pool helper
 * exclusive set. Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1530_marker[] = "libcgj-batch1530";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1530 — report this TU's graph batch number.
 *
 * Always returns 1530.
 */
uint32_t
gj_batch_id_1530(void)
{
	return 1530u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_1530(void)
    __attribute__((alias("gj_batch_id_1530")));
