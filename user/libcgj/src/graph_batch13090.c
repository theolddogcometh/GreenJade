/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13090: exclusive bar3 checklist soft batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13090(void);
 *     - Returns the compile-time graph batch number for this TU (13090).
 *   uint32_t __gj_batch_id_13090  (alias)
 *   __libcgj_batch13090_marker = "libcgj-batch13090"
 *
 * Exclusive continuum CREATE-ONLY (13081-13090: bar3 checklist soft
 * stubs — all→0). Unique gj_batch_id_13090 surface only; no multi-def.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols. Distinct from gj_batch_id_12890. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13090_marker[] = "libcgj-batch13090";

#define B13090_BATCH_ID  13090u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13090_id(void)
{
	return B13090_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13090 - report this TU's graph batch number.
 *
 * Always returns 13090. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_13090(void)
{
	(void)NULL;
	return b13090_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13090(void)
    __attribute__((alias("gj_batch_id_13090")));
