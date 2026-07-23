/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch830: milestone batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_830(void);
 *     — Returns the compile-time graph batch number for this TU (830).
 *   uint32_t __gj_batch_id_830  (alias)
 *   __libcgj_batch830_marker = "libcgj-batch830"
 *
 * Does NOT define gj_batch_id / prior batch_id_* or milestone_* surfaces
 * — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch830_marker[] = "libcgj-batch830";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_830 — report this TU's graph batch number.
 *
 * Always returns 830.
 */
uint32_t
gj_batch_id_830(void)
{
	return 830u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_830(void)
    __attribute__((alias("gj_batch_id_830")));
