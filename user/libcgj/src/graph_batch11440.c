/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11440: exclusive IM soft batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11440(void);
 *     - Returns the compile-time graph batch number for this TU (11440).
 *   uint32_t __gj_batch_id_11440  (alias)
 *   __libcgj_batch11440_marker = "libcgj-batch11440"
 *
 * Exclusive continuum CREATE-ONLY (11431-11440: IM soft stubs —
 * ok slots all→0; im_soft_ready→1; batch_id→11440). Unique
 * gj_batch_id_11440 surface only; no multi-def. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11440_marker[] = "libcgj-batch11440";

#define B11440_BATCH_ID  11440u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11440_id(void)
{
	return B11440_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11440 - report this TU's graph batch number.
 *
 * Always returns 11440. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_11440(void)
{
	(void)NULL;
	return b11440_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_11440(void)
    __attribute__((alias("gj_batch_id_11440")));
