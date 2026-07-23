/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12630: mesa soft stubs batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12630(void);
 *     - Returns the compile-time graph batch number for this TU (12630).
 *   uint32_t __gj_batch_id_12630  (alias)
 *   __libcgj_batch12630_marker = "libcgj-batch12630"
 *
 * Exclusive continuum CREATE-ONLY (12621-12630: mesa soft stubs —
 * all soft lamps →0; batch_id→12630). Unique gj_batch_id_12630 surface
 * only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * Closes the 12621-12630 continuum (mesa/radv/amdvlk/anv/lavapipe/
 * zink/iris/v3d soft ok stubs + mesa soft ready). Distinct from
 * gj_batch_id_12430, gj_batch_id_12230, gj_batch_id_12030,
 * gj_batch_id_11930, gj_batch_id_11730, gj_batch_id_11530,
 * gj_batch_id_11330, gj_batch_id_11130, gj_batch_id_10930,
 * gj_batch_id_10730 and gj_batch_id_10530. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12630_marker[] = "libcgj-batch12630";

#define B12630_BATCH_ID  12630u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12630_id(void)
{
	return B12630_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12630 - report this TU's graph batch number.
 *
 * Always returns 12630. Soft pure-data identity for the mesa soft
 * stubs continuum close. Does not call libc. No parent wires.
 */
uint32_t
gj_batch_id_12630(void)
{
	(void)NULL;
	return b12630_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12630(void)
    __attribute__((alias("gj_batch_id_12630")));
