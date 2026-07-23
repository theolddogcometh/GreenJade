/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1420: batch identity marker (milestone).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1420(void);
 *     — Returns the compile-time graph batch number for this TU (1420).
 *   uint32_t __gj_batch_id_1420  (alias)
 *   __libcgj_batch1420_marker = "libcgj-batch1420"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1420_marker[] = "libcgj-batch1420";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1420 — report this TU's graph batch number.
 *
 * Always returns 1420.
 */
uint32_t
gj_batch_id_1420(void)
{
	return 1420u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_1420(void)
    __attribute__((alias("gj_batch_id_1420")));
