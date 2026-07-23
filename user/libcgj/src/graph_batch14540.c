/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14540: exclusive hda mixer soft names batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14540(void);
 *     - Returns the compile-time graph batch number for this TU (14540).
 *   uint32_t __gj_batch_id_14540  (alias)
 *   __libcgj_batch14540_marker = "libcgj-batch14540"
 *
 * MILESTONE 14540 for the exclusive continuum CREATE-ONLY wave
 * (batches 14531-14539: hda mixer soft name tags). Does NOT
 * redefine gj_batch_id / prior batch_id_* symbols - avoid multi-def.
 * Distinct from gj_batch_id_14440 and sibling batch_id soft stubs.
 * No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14540_marker[] = "libcgj-batch14540";

/* Exclusive hda mixer soft names batch id. */
#define B14540_ID  14540u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14540_id(void)
{
	return B14540_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14540 - soft continuum surface
 *
 * Always returns 14540u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_14540(void)
{
	(void)NULL;
	return b14540_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14540(void)
    __attribute__((alias("gj_batch_id_14540")));
