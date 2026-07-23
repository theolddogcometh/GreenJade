/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14550: exclusive scsi mid soft batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14550(void);
 *     - Returns the compile-time graph batch number for this TU (14550).
 *   uint32_t __gj_batch_id_14550  (alias)
 *   __libcgj_batch14550_marker = "libcgj-batch14550"
 *
 * MILESTONE 14550 for the exclusive continuum CREATE-ONLY wave
 * (batches 14541-14549: scsi mid soft stubs all→0). Does NOT
 * redefine gj_batch_id / prior batch_id_* symbols - avoid multi-def.
 * Distinct from gj_batch_id_14450 and sibling batch_id soft stubs.
 * No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14550_marker[] = "libcgj-batch14550";

/* Exclusive scsi mid soft batch id. */
#define B14550_ID  14550u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14550_id(void)
{
	return B14550_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14550 - soft continuum surface
 *
 * Always returns 14550u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_14550(void)
{
	(void)NULL;
	return b14550_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14550(void)
    __attribute__((alias("gj_batch_id_14550")));
