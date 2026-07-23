/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14490: exclusive bar3 checklist soft batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14490(void);
 *     - Returns the compile-time graph batch number for this TU (14490).
 *   uint32_t __gj_batch_id_14490  (alias)
 *   __libcgj_batch14490_marker = "libcgj-batch14490"
 *
 * MILESTONE 14490 for the exclusive continuum CREATE-ONLY wave
 * (batches 14481-14489: bar3 checklist soft stubs all→0). Does NOT
 * redefine gj_batch_id / prior batch_id_* symbols - avoid multi-def.
 * Distinct from gj_batch_id_14390, gj_batch_id_14480, and sibling
 * batch_id soft stubs in this wave. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14490_marker[] = "libcgj-batch14490";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14490_id(void)
{
	return 14490u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14490 - report this TU's graph batch number.
 *
 * Always returns 14490.
 */
uint32_t
gj_batch_id_14490(void)
{
	(void)NULL;
	return b14490_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14490(void)
    __attribute__((alias("gj_batch_id_14490")));
