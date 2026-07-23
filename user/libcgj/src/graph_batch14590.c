/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14590: exclusive bar3 checklist soft batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14590(void);
 *     - Returns the compile-time graph batch number for this TU (14590).
 *   uint32_t __gj_batch_id_14590  (alias)
 *   __libcgj_batch14590_marker = "libcgj-batch14590"
 *
 * MILESTONE 14590 for the exclusive continuum CREATE-ONLY wave
 * (batches 14581-14589: bar3 checklist soft stubs all→0). Does NOT
 * redefine gj_batch_id / prior batch_id_* symbols - avoid multi-def.
 * Distinct from gj_batch_id_14490 and sibling batch_id soft stubs.
 * No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14590_marker[] = "libcgj-batch14590";

/* Exclusive bar3 checklist soft batch id. */
#define B14590_ID  14590u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14590_id(void)
{
	return B14590_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14590 - soft continuum surface
 *
 * Always returns 14590u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_14590(void)
{
	(void)NULL;
	return b14590_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14590(void)
    __attribute__((alias("gj_batch_id_14590")));
