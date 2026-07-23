/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10440: exclusive IM soft batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10440(void);
 *     - Returns the compile-time graph batch number for this TU (10440).
 *   uint32_t __gj_batch_id_10440  (alias)
 *   __libcgj_batch10440_marker = "libcgj-batch10440"
 *
 * Exclusive continuum CREATE-ONLY (10431-10440: IM soft stubs —
 * ok slots all→0; im_soft_ready→1; batch_id→10440). Unique
 * gj_batch_id_10440 surface only; no multi-def. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10440_marker[] = "libcgj-batch10440";

#define B10440_BATCH_ID  10440u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10440_id(void)
{
	return B10440_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10440 - report this TU's graph batch number.
 *
 * Always returns 10440. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_10440(void)
{
	(void)NULL;
	return b10440_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10440(void)
    __attribute__((alias("gj_batch_id_10440")));
