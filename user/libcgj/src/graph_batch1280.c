/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1280: batch identity marker.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1280(void);
 *     — Returns the compile-time graph batch number for this TU (1280).
 *   uint32_t __gj_batch_id_1280  (alias)
 *   __libcgj_batch1280_marker = "libcgj-batch1280"
 *
 * Does NOT define gj_batch_id / prior batch_id_* — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1280_marker[] = "libcgj-batch1280";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1280 — report this TU's graph batch number.
 *
 * Always returns 1280.
 */
uint32_t
gj_batch_id_1280(void)
{
	return 1280u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_1280(void)
    __attribute__((alias("gj_batch_id_1280")));
