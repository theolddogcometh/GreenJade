/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11360: exclusive controller soft batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11360(void);
 *     - Returns the compile-time graph batch number for this TU (11360).
 *   uint32_t __gj_batch_id_11360  (alias)
 *   __libcgj_batch11360_marker = "libcgj-batch11360"
 *
 * Exclusive continuum CREATE-ONLY (11351-11360: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→11360). Unique
 * gj_batch_id_11360 surface only; no multi-def. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11360_marker[] = "libcgj-batch11360";

#define B11360_BATCH_ID  11360u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11360_id(void)
{
	return B11360_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11360 - report this TU's graph batch number.
 *
 * Always returns 11360. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_11360(void)
{
	(void)NULL;
	return b11360_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_11360(void)
    __attribute__((alias("gj_batch_id_11360")));
