/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14420: exclusive bar3 readiness soft batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14420(void);
 *     - Returns the compile-time graph batch number for this TU (14420).
 *   uint32_t __gj_batch_id_14420  (alias)
 *   __libcgj_batch14420_marker = "libcgj-batch14420"
 *
 * MILESTONE 14420 for the exclusive continuum CREATE-ONLY wave
 * (batches 14411-14419: bar3 readiness soft stubs all→0). Does NOT
 * redefine gj_batch_id / prior batch_id_* symbols - avoid multi-def.
 * Distinct from gj_batch_id_14320, gj_batch_id_14410, and sibling
 * batch_id soft stubs in this wave. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14420_marker[] = "libcgj-batch14420";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14420_id(void)
{
	return 14420u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14420 - report this TU's graph batch number.
 *
 * Always returns 14420.
 */
uint32_t
gj_batch_id_14420(void)
{
	(void)NULL;
	return b14420_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14420(void)
    __attribute__((alias("gj_batch_id_14420")));
