/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8060: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8060(void);
 *     - Returns the compile-time graph batch number for this TU (8060).
 *   uint32_t __gj_batch_id_8060  (alias)
 *   __libcgj_batch8060_marker = "libcgj-batch8060"
 *
 * Exclusive continuum CREATE-ONLY (8051-8060: memset pattern stubs —
 * memset_zero_id, memset_ff_id, pat_byte, pat_is_zero, pat_is_ff,
 * len_ok, pat_word, pat_ok, len_errorish, batch_id_8060).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8060_marker[] = "libcgj-batch8060";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8060_id(void)
{
	return 8060u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8060 - report this TU's graph batch number.
 *
 * Always returns 8060. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_8060(void)
{
	(void)NULL;
	return b8060_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8060(void)
    __attribute__((alias("gj_batch_id_8060")));
