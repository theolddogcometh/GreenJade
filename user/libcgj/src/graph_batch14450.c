/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14450: exclusive scsi mid soft batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14450(void);
 *     - Returns the compile-time graph batch number for this TU (14450).
 *   uint32_t __gj_batch_id_14450  (alias)
 *   __libcgj_batch14450_marker = "libcgj-batch14450"
 *
 * MILESTONE 14450 for the exclusive continuum CREATE-ONLY wave
 * (batches 14441-14449: scsi mid soft stubs all→0). Does NOT
 * redefine gj_batch_id / prior batch_id_* symbols - avoid multi-def.
 * Distinct from gj_batch_id_14350, gj_batch_id_14440, and sibling
 * batch_id soft stubs in this wave. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14450_marker[] = "libcgj-batch14450";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14450_id(void)
{
	return 14450u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14450 - report this TU's graph batch number.
 *
 * Always returns 14450.
 */
uint32_t
gj_batch_id_14450(void)
{
	(void)NULL;
	return b14450_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14450(void)
    __attribute__((alias("gj_batch_id_14450")));
