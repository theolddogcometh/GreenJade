/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14560: exclusive pmm order soft batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14560(void);
 *     - Returns the compile-time graph batch number for this TU (14560).
 *   uint32_t __gj_batch_id_14560  (alias)
 *   __libcgj_batch14560_marker = "libcgj-batch14560"
 *
 * MILESTONE 14560 for the exclusive continuum CREATE-ONLY wave
 * (batches 14551-14559: pmm order soft tags). Does NOT
 * redefine gj_batch_id / prior batch_id_* symbols - avoid multi-def.
 * Distinct from gj_batch_id_14460 and sibling batch_id soft stubs.
 * No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14560_marker[] = "libcgj-batch14560";

/* Exclusive pmm order soft batch id. */
#define B14560_ID  14560u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14560_id(void)
{
	return B14560_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14560 - soft continuum surface
 *
 * Always returns 14560u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_14560(void)
{
	(void)NULL;
	return b14560_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14560(void)
    __attribute__((alias("gj_batch_id_14560")));
