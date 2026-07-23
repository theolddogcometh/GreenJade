/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14640: exclusive hda mixer soft names soft batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14640(void);
 *     - Returns the compile-time graph batch number for this TU (14640).
 *   uint32_t __gj_batch_id_14640  (alias)
 *   __libcgj_batch14640_marker = "libcgj-batch14640"
 *
 * MILESTONE 14640 for the exclusive continuum CREATE-ONLY wave
 * (batches 14631-14639: hda mixer soft name tags). Does NOT
 * redefine gj_batch_id / prior batch_id_* symbols - avoid multi-def.
 * Distinct from gj_batch_id_14540 and sibling batch_id soft stubs.
 * No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14640_marker[] = "libcgj-batch14640";

/* Exclusive hda mixer soft names soft batch id. */
#define B14640_ID  14640u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14640_id(void)
{
	return B14640_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14640 - soft continuum surface
 *
 * Always returns 14640u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_14640(void)
{
	(void)NULL;
	return b14640_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14640(void)
    __attribute__((alias("gj_batch_id_14640")));
