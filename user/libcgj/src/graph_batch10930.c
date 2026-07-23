/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10930: mesa soft stubs batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10930(void);
 *     - Returns the compile-time graph batch number for this TU (10930).
 *   uint32_t __gj_batch_id_10930  (alias)
 *   __libcgj_batch10930_marker = "libcgj-batch10930"
 *
 * Exclusive continuum CREATE-ONLY (10921-10930: mesa soft stubs —
 * all soft lamps →0; batch_id→10930). Unique gj_batch_id_10930 surface
 * only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * Closes the 10921-10930 continuum (mesa/radv/amdvlk/anv/lavapipe/
 * zink/iris/v3d soft ok stubs + mesa soft ready). Distinct from
 * gj_batch_id_10730 and gj_batch_id_10530. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10930_marker[] = "libcgj-batch10930";

#define B10930_BATCH_ID  10930u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10930_id(void)
{
	return B10930_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10930 - report this TU's graph batch number.
 *
 * Always returns 10930. Soft pure-data identity for the mesa soft
 * stubs continuum close. Does not call libc. No parent wires.
 */
uint32_t
gj_batch_id_10930(void)
{
	(void)NULL;
	return b10930_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10930(void)
    __attribute__((alias("gj_batch_id_10930")));
