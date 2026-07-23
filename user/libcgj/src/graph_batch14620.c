/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14620: exclusive bar3 readiness soft batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14620(void);
 *     - Returns the compile-time graph batch number for this TU (14620).
 *   uint32_t __gj_batch_id_14620  (alias)
 *   __libcgj_batch14620_marker = "libcgj-batch14620"
 *
 * MILESTONE 14620 for the exclusive continuum CREATE-ONLY wave
 * (batches 14611-14619: bar3 readiness soft stubs all→0). Does NOT
 * redefine gj_batch_id / prior batch_id_* symbols - avoid multi-def.
 * Distinct from gj_batch_id_14520 and sibling batch_id soft stubs.
 * No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14620_marker[] = "libcgj-batch14620";

/* Exclusive bar3 readiness soft batch id. */
#define B14620_ID  14620u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14620_id(void)
{
	return B14620_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14620 - soft continuum surface
 *
 * Always returns 14620u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_14620(void)
{
	(void)NULL;
	return b14620_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14620(void)
    __attribute__((alias("gj_batch_id_14620")));
