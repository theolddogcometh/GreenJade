/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14430: exclusive steam media soft batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14430(void);
 *     - Returns the compile-time graph batch number for this TU (14430).
 *   uint32_t __gj_batch_id_14430  (alias)
 *   __libcgj_batch14430_marker = "libcgj-batch14430"
 *
 * MILESTONE 14430 for the exclusive continuum CREATE-ONLY wave
 * (batches 14421-14429: steam media soft stubs all→0). Does NOT
 * redefine gj_batch_id / prior batch_id_* symbols - avoid multi-def.
 * Distinct from gj_batch_id_14330, gj_batch_id_14420, and sibling
 * batch_id soft stubs in this wave. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14430_marker[] = "libcgj-batch14430";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14430_id(void)
{
	return 14430u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14430 - report this TU's graph batch number.
 *
 * Always returns 14430.
 */
uint32_t
gj_batch_id_14430(void)
{
	(void)NULL;
	return b14430_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14430(void)
    __attribute__((alias("gj_batch_id_14430")));
