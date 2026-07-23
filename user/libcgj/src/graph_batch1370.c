/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1370: batch identity marker (milestone).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1370(void);
 *     — Returns the compile-time graph batch number for this TU (1370).
 *   uint32_t __gj_batch_id_1370  (alias)
 *   __libcgj_batch1370_marker = "libcgj-batch1370"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1370_marker[] = "libcgj-batch1370";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1370 — report this TU's graph batch number.
 *
 * Always returns 1370.
 */
uint32_t
gj_batch_id_1370(void)
{
	return 1370u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_1370(void)
    __attribute__((alias("gj_batch_id_1370")));
