/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14580: exclusive notify/udx soft batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14580(void);
 *     - Returns the compile-time graph batch number for this TU (14580).
 *   uint32_t __gj_batch_id_14580  (alias)
 *   __libcgj_batch14580_marker = "libcgj-batch14580"
 *
 * MILESTONE 14580 for the exclusive continuum CREATE-ONLY wave
 * (batches 14571-14579: udx soft stubs all→0). Does NOT
 * redefine gj_batch_id / prior batch_id_* symbols - avoid multi-def.
 * Distinct from gj_batch_id_14480 and sibling batch_id soft stubs.
 * No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14580_marker[] = "libcgj-batch14580";

/* Exclusive notify/udx soft batch id. */
#define B14580_ID  14580u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14580_id(void)
{
	return B14580_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14580 - soft continuum surface
 *
 * Always returns 14580u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_14580(void)
{
	(void)NULL;
	return b14580_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14580(void)
    __attribute__((alias("gj_batch_id_14580")));
