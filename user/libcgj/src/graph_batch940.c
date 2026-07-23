/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch940: milestone batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_940(void);
 *     — Returns the compile-time graph batch number for this TU (940).
 *   uint32_t __gj_batch_id_940  (alias)
 *   __libcgj_batch940_marker = "libcgj-batch940"

 *
 * Does NOT define gj_batch_id / gj_batch_id_930 / prior batch_id_* —
 * avoid multi-def.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch940_marker[] = "libcgj-batch940";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_940 — report this TU's graph batch number.
 *
 * Always returns 940.
 */
uint32_t
gj_batch_id_940(void)
{
	return 940u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_940(void)
    __attribute__((alias("gj_batch_id_940")));
