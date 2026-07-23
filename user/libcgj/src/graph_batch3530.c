/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3530: batch identity for sort algorithms wave.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3530(void);
 *     - Returns the compile-time graph batch number for this TU (3530).
 *   uint32_t __gj_batch_id_3530  (alias)
 *   __libcgj_batch3530_marker = "libcgj-batch3530"
 *
 * Exclusive sort algorithms wave (3521-3530) terminus. Distinct from
 * gj_batch_id / prior gj_batch_id_* symbols — avoid multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3530_marker[] = "libcgj-batch3530";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3530_id(void)
{
	return 3530u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3530 - report this TU's graph batch number.
 *
 * Always returns 3530.
 */
uint32_t
gj_batch_id_3530(void)
{
	(void)NULL;
	return b3530_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3530(void)
    __attribute__((alias("gj_batch_id_3530")));
