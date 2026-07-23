/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14690: exclusive bar3 checklist soft batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14690(void);
 *     - Returns the compile-time graph batch number for this TU (14690).
 *   uint32_t __gj_batch_id_14690  (alias)
 *   __libcgj_batch14690_marker = "libcgj-batch14690"
 *
 * MILESTONE 14690 for the exclusive continuum CREATE-ONLY wave
 * (batches 14681-14689: bar3 checklist soft stubs all→0). Does NOT
 * redefine gj_batch_id / prior batch_id_* symbols - avoid multi-def.
 * Distinct from gj_batch_id_14590 and sibling batch_id soft stubs.
 * No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14690_marker[] = "libcgj-batch14690";

/* Exclusive bar3 checklist soft batch id. */
#define B14690_ID  14690u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14690_id(void)
{
	return B14690_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14690 - soft continuum surface
 *
 * Always returns 14690u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_14690(void)
{
	(void)NULL;
	return b14690_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14690(void)
    __attribute__((alias("gj_batch_id_14690")));
