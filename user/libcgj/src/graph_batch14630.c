/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14630: exclusive steam media soft batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14630(void);
 *     - Returns the compile-time graph batch number for this TU (14630).
 *   uint32_t __gj_batch_id_14630  (alias)
 *   __libcgj_batch14630_marker = "libcgj-batch14630"
 *
 * MILESTONE 14630 for the exclusive continuum CREATE-ONLY wave
 * (batches 14621-14629: steam media soft stubs all→0). Does NOT
 * redefine gj_batch_id / prior batch_id_* symbols - avoid multi-def.
 * Distinct from gj_batch_id_14530 and sibling batch_id soft stubs.
 * No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14630_marker[] = "libcgj-batch14630";

/* Exclusive steam media soft batch id. */
#define B14630_ID  14630u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14630_id(void)
{
	return B14630_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14630 - soft continuum surface
 *
 * Always returns 14630u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_14630(void)
{
	(void)NULL;
	return b14630_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14630(void)
    __attribute__((alias("gj_batch_id_14630")));
