/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1260: batch identity marker.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1260(void);
 *     — Returns the compile-time graph batch number for this TU (1260).
 *   uint32_t __gj_batch_id_1260  (alias)
 *   __libcgj_batch1260_marker = "libcgj-batch1260"
 *
 * Does NOT define gj_batch_id / prior batch_id_* — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1260_marker[] = "libcgj-batch1260";

/*
 * gj_batch_id_1260 — report this TU's graph batch number.
 *
 * Always returns 1260.
 */
uint32_t
gj_batch_id_1260(void)
{
	return 1260u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_1260(void)
    __attribute__((alias("gj_batch_id_1260")));
