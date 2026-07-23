/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1180: batch identity marker.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1180(void);
 *     — Returns the compile-time graph batch number for this TU (1180).
 *   uint32_t __gj_batch_id_1180  (alias)
 *   __libcgj_batch1180_marker = "libcgj-batch1180"
 *
 * Does NOT define gj_batch_id / prior batch_id_* — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1180_marker[] = "libcgj-batch1180";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1180 — report this TU's graph batch number.
 *
 * Always returns 1180.
 */
uint32_t
gj_batch_id_1180(void)
{
	return 1180u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_1180(void)
    __attribute__((alias("gj_batch_id_1180")));
