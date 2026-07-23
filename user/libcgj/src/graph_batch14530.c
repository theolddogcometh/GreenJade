/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14530: exclusive steam media soft batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14530(void);
 *     - Returns the compile-time graph batch number for this TU (14530).
 *   uint32_t __gj_batch_id_14530  (alias)
 *   __libcgj_batch14530_marker = "libcgj-batch14530"
 *
 * MILESTONE 14530 for the exclusive continuum CREATE-ONLY wave
 * (batches 14521-14529: steam media soft stubs all→0). Does NOT
 * redefine gj_batch_id / prior batch_id_* symbols - avoid multi-def.
 * Distinct from gj_batch_id_14430 and sibling batch_id soft stubs.
 * No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14530_marker[] = "libcgj-batch14530";

/* Exclusive steam media soft batch id. */
#define B14530_ID  14530u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14530_id(void)
{
	return B14530_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14530 - soft continuum surface
 *
 * Always returns 14530u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_14530(void)
{
	(void)NULL;
	return b14530_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14530(void)
    __attribute__((alias("gj_batch_id_14530")));
