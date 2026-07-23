/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8140: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8140(void);
 *     - Returns the compile-time graph batch number for this TU (8140).
 *   uint32_t __gj_batch_id_8140  (alias)
 *   __libcgj_batch8140_marker = "libcgj-batch8140"
 *
 * Exclusive continuum CREATE-ONLY (8131-8140: strcat/strncat stubs —
 * src_empty, dst_null, n_zero, n_pos, take, cat_need, ncat_need,
 * kind_unbounded, kind_bounded, batch_id_8140).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8140_marker[] = "libcgj-batch8140";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8140_id(void)
{
	return 8140u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8140 - report this TU's graph batch number.
 *
 * Always returns 8140. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_8140(void)
{
	(void)NULL;
	return b8140_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8140(void)
    __attribute__((alias("gj_batch_id_8140")));
