/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1740: milestone 1740 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1740(void);
 *     — Returns the compile-time graph batch number for this TU (1740).
 *       Milestone for the decimal/fixed-print exclusive theme
 *       (batches 1731–1739).
 *   uint32_t __gj_batch_id_1740  (alias)
 *   __libcgj_batch1740_marker = "libcgj-batch1740"
 *
 * Does NOT redefine prior batch_id_* symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1740_marker[] = "libcgj-batch1740";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1740 — report this TU's graph batch number.
 *
 * Always returns 1740.
 */
uint32_t
gj_batch_id_1740(void)
{
	return 1740u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_1740(void)
    __attribute__((alias("gj_batch_id_1740")));
