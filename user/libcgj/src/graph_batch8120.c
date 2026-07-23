/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8120: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8120(void);
 *     - Returns the compile-time graph batch number for this TU (8120).
 *   uint32_t __gj_batch_id_8120  (alias)
 *   __libcgj_batch8120_marker = "libcgj-batch8120"
 *
 * Exclusive continuum CREATE-ONLY (8111-8120: strcpy bounds stubs —
 * min_dst_id, nul_bytes_id, empty_need_id, dst_is_zero, need_fits,
 * dst_ok, need_ok, need_from_len, need_errorish, batch_id_8120).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8120_marker[] = "libcgj-batch8120";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8120_id(void)
{
	return 8120u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8120 - report this TU's graph batch number.
 *
 * Always returns 8120. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_8120(void)
{
	(void)NULL;
	return b8120_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8120(void)
    __attribute__((alias("gj_batch_id_8120")));
