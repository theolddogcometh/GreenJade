/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11560: exclusive controller soft batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11560(void);
 *     - Returns the compile-time graph batch number for this TU (11560).
 *   uint32_t __gj_batch_id_11560  (alias)
 *   __libcgj_batch11560_marker = "libcgj-batch11560"
 *
 * Exclusive continuum CREATE-ONLY (11551-11560: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→11560). Unique
 * gj_batch_id_11560 surface only; no multi-def. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11560_marker[] = "libcgj-batch11560";

#define B11560_BATCH_ID  11560u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11560_id(void)
{
	return B11560_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11560 - report this TU's graph batch number.
 *
 * Always returns 11560. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_11560(void)
{
	(void)NULL;
	return b11560_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_11560(void)
    __attribute__((alias("gj_batch_id_11560")));
