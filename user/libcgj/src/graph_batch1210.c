/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1210: batch identity marker.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1210(void);
 *     — Returns the compile-time graph batch number for this TU (1210).
 *   uint32_t __gj_batch_id_1210  (alias)
 *   __libcgj_batch1210_marker = "libcgj-batch1210"
 *
 * Does NOT define gj_batch_id / prior batch_id_* — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1210_marker[] = "libcgj-batch1210";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1210 — report this TU's graph batch number.
 *
 * Always returns 1210.
 */
uint32_t
gj_batch_id_1210(void)
{
	return 1210u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_1210(void)
    __attribute__((alias("gj_batch_id_1210")));
