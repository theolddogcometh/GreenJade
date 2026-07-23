/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14520: exclusive bar3 readiness soft batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14520(void);
 *     - Returns the compile-time graph batch number for this TU (14520).
 *   uint32_t __gj_batch_id_14520  (alias)
 *   __libcgj_batch14520_marker = "libcgj-batch14520"
 *
 * MILESTONE 14520 for the exclusive continuum CREATE-ONLY wave
 * (batches 14511-14519: bar3 readiness soft stubs all→0). Does NOT
 * redefine gj_batch_id / prior batch_id_* symbols - avoid multi-def.
 * Distinct from gj_batch_id_14420 and sibling batch_id soft stubs.
 * No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14520_marker[] = "libcgj-batch14520";

/* Exclusive bar3 readiness soft batch id. */
#define B14520_ID  14520u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14520_id(void)
{
	return B14520_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14520 - soft continuum surface
 *
 * Always returns 14520u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_14520(void)
{
	(void)NULL;
	return b14520_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14520(void)
    __attribute__((alias("gj_batch_id_14520")));
