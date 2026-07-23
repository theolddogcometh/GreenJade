/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1580: milestone batch identity marker.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1580(void);
 *     — Returns the compile-time graph batch number for this TU (1580).
 *       Milestone for the bit-matrix / flags32 exclusive wave
 *       (batches 1571–1580).
 *   uint32_t __gj_batch_id_1580  (alias)
 *   __libcgj_batch1580_marker = "libcgj-batch1580"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1580_marker[] = "libcgj-batch1580";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1580 — report this TU's graph batch number.
 *
 * Always returns 1580.
 */
uint32_t
gj_batch_id_1580(void)
{
	return 1580u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_1580(void)
    __attribute__((alias("gj_batch_id_1580")));
