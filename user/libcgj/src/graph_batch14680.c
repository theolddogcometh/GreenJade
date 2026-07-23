/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14680: exclusive notify/udx soft batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14680(void);
 *     - Returns the compile-time graph batch number for this TU (14680).
 *   uint32_t __gj_batch_id_14680  (alias)
 *   __libcgj_batch14680_marker = "libcgj-batch14680"
 *
 * MILESTONE 14680 for the exclusive continuum CREATE-ONLY wave
 * (batches 14671-14679: udx soft stubs all→0). Does NOT
 * redefine gj_batch_id / prior batch_id_* symbols - avoid multi-def.
 * Distinct from gj_batch_id_14580 and sibling batch_id soft stubs.
 * No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14680_marker[] = "libcgj-batch14680";

/* Exclusive notify/udx soft batch id. */
#define B14680_ID  14680u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14680_id(void)
{
	return B14680_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14680 - soft continuum surface
 *
 * Always returns 14680u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_14680(void)
{
	(void)NULL;
	return b14680_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14680(void)
    __attribute__((alias("gj_batch_id_14680")));
