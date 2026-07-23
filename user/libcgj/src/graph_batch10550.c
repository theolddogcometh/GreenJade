/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10550: exclusive gamescope soft batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10550(void);
 *     - Returns the compile-time graph batch number for this TU (10550).
 *   uint32_t __gj_batch_id_10550  (alias)
 *   __libcgj_batch10550_marker = "libcgj-batch10550"
 *
 * Exclusive continuum CREATE-ONLY (10541-10550: gamescope soft stubs —
 * all→0). Unique gj_batch_id_10550 surface only; no multi-def.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10550_marker[] = "libcgj-batch10550";

#define B10550_BATCH_ID  10550u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10550_id(void)
{
	return B10550_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10550 - report this TU's graph batch number.
 *
 * Always returns 10550. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_10550(void)
{
	(void)NULL;
	return b10550_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10550(void)
    __attribute__((alias("gj_batch_id_10550")));
