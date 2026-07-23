/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch930: milestone batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_930(void);
 *     — Returns the compile-time graph batch number for this TU (930).
 *   uint32_t __gj_batch_id_930  (alias)
 *   __libcgj_batch930_marker = "libcgj-batch930"

 *
 * Does NOT define gj_batch_id / prior batch_id_* — avoid multi-def.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch930_marker[] = "libcgj-batch930";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_930 — report this TU's graph batch number.
 *
 * Always returns 930.
 */
uint32_t
gj_batch_id_930(void)
{
	return 930u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_930(void)
    __attribute__((alias("gj_batch_id_930")));
