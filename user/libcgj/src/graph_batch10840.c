/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10840: exclusive IM soft batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10840(void);
 *     - Returns the compile-time graph batch number for this TU (10840).
 *   uint32_t __gj_batch_id_10840  (alias)
 *   __libcgj_batch10840_marker = "libcgj-batch10840"
 *
 * Exclusive continuum CREATE-ONLY (10831-10840: IM soft stubs —
 * ok slots all→0; im_soft_ready→1; batch_id→10840). Unique
 * gj_batch_id_10840 surface only; no multi-def. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10840_marker[] = "libcgj-batch10840";

#define B10840_BATCH_ID  10840u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10840_id(void)
{
	return B10840_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10840 - report this TU's graph batch number.
 *
 * Always returns 10840. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_10840(void)
{
	(void)NULL;
	return b10840_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10840(void)
    __attribute__((alias("gj_batch_id_10840")));
