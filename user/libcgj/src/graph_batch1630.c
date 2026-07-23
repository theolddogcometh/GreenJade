/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1630: string case/encoding milestone marker.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1630(void);
 *     — Returns the compile-time graph batch number for this TU (1630).
 *       Milestone for the ASCII string case/encoding exclusive series
 *       (batches 1621–1629: to_lower/to_upper/swapcase/is_/mem_/fold_eq).
 *   uint32_t __gj_batch_id_1630  (alias)
 *   __libcgj_batch1630_marker = "libcgj-batch1630"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1630_marker[] = "libcgj-batch1630";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1630 — report this TU's graph batch number.
 *
 * Always returns 1630.
 */
uint32_t
gj_batch_id_1630(void)
{
	return 1630u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_1630(void)
    __attribute__((alias("gj_batch_id_1630")));
