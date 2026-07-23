/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14670: exclusive aarch64 soft batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14670(void);
 *     - Returns the compile-time graph batch number for this TU (14670).
 *   uint32_t __gj_batch_id_14670  (alias)
 *   __libcgj_batch14670_marker = "libcgj-batch14670"
 *
 * MILESTONE 14670 for the exclusive continuum CREATE-ONLY wave
 * (batches 14661-14669: aarch64 soft stubs all→0). Does NOT
 * redefine gj_batch_id / prior batch_id_* symbols - avoid multi-def.
 * Distinct from gj_batch_id_14570 and sibling batch_id soft stubs.
 * No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14670_marker[] = "libcgj-batch14670";

/* Exclusive aarch64 soft batch id. */
#define B14670_ID  14670u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14670_id(void)
{
	return B14670_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14670 - soft continuum surface
 *
 * Always returns 14670u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_14670(void)
{
	(void)NULL;
	return b14670_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14670(void)
    __attribute__((alias("gj_batch_id_14670")));
