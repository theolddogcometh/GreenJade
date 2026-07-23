/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14660: exclusive large-ram 768G soak soft batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14660(void);
 *     - Returns the compile-time graph batch number for this TU (14660).
 *   uint32_t __gj_batch_id_14660  (alias)
 *   __libcgj_batch14660_marker = "libcgj-batch14660"
 *
 * MILESTONE 14660 for the exclusive continuum CREATE-ONLY wave
 * (batches 14651-14659: large-ram 768G soak soft markers). Does NOT
 * redefine gj_batch_id / prior batch_id_* symbols - avoid multi-def.
 * Distinct from gj_batch_id_14560 and sibling batch_id soft stubs.
 * No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14660_marker[] = "libcgj-batch14660";

/* Exclusive large-ram 768G soak soft batch id. */
#define B14660_ID  14660u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14660_id(void)
{
	return B14660_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14660 - soft continuum surface
 *
 * Always returns 14660u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_14660(void)
{
	(void)NULL;
	return b14660_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14660(void)
    __attribute__((alias("gj_batch_id_14660")));
