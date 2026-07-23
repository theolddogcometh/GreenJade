/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1390: batch identity marker.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1390(void);
 *     — Returns the compile-time graph batch number for this TU (1390).
 *   uint32_t __gj_batch_id_1390  (alias)
 *   __libcgj_batch1390_marker = "libcgj-batch1390"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1390_marker[] = "libcgj-batch1390";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1390 — report this TU's graph batch number.
 *
 * Always returns 1390.
 */
uint32_t
gj_batch_id_1390(void)
{
	return 1390u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_1390(void)
    __attribute__((alias("gj_batch_id_1390")));
