/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1470: batch identity marker (milestone).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1470(void);
 *     — Returns the compile-time graph batch number for this TU (1470).
 *   uint32_t __gj_batch_id_1470  (alias)
 *   __libcgj_batch1470_marker = "libcgj-batch1470"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1470_marker[] = "libcgj-batch1470";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1470 — report this TU's graph batch number.
 *
 * Always returns 1470.
 */
uint32_t
gj_batch_id_1470(void)
{
	return 1470u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_1470(void)
    __attribute__((alias("gj_batch_id_1470")));
