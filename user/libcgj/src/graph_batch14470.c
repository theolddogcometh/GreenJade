/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14470: exclusive aarch64 soft batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14470(void);
 *     - Returns the compile-time graph batch number for this TU (14470).
 *   uint32_t __gj_batch_id_14470  (alias)
 *   __libcgj_batch14470_marker = "libcgj-batch14470"
 *
 * MILESTONE 14470 for the exclusive continuum CREATE-ONLY wave
 * (batches 14461-14469: aarch64 soft stubs all→0). Does NOT
 * redefine gj_batch_id / prior batch_id_* symbols - avoid multi-def.
 * Distinct from gj_batch_id_14370, gj_batch_id_14460, and sibling
 * batch_id soft stubs in this wave. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14470_marker[] = "libcgj-batch14470";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14470_id(void)
{
	return 14470u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14470 - report this TU's graph batch number.
 *
 * Always returns 14470.
 */
uint32_t
gj_batch_id_14470(void)
{
	(void)NULL;
	return b14470_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14470(void)
    __attribute__((alias("gj_batch_id_14470")));
