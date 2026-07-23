/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7370: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7370(void);
 *     - Returns the compile-time graph batch number for this TU (7370).
 *   uint32_t __gj_batch_id_7370  (alias)
 *   __libcgj_batch7370_marker = "libcgj-batch7370"
 *
 * Exclusive continuum CREATE-ONLY (7361-7370: fallocate mode stubs —
 * keep_size_id, punch_hole_id, collapse_range_id, zero_range_id,
 * has_keep_size, has_punch_hole, modes_ok, modes_pack, modes_errorish,
 * batch_id_7370).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7370_marker[] = "libcgj-batch7370";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7370_id(void)
{
	return 7370u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7370 - report this TU's graph batch number.
 *
 * Always returns 7370. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_7370(void)
{
	(void)NULL;
	return b7370_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_7370(void)
    __attribute__((alias("gj_batch_id_7370")));
