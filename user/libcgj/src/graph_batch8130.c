/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8130: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8130(void);
 *     - Returns the compile-time graph batch number for this TU (8130).
 *   uint32_t __gj_batch_id_8130  (alias)
 *   __libcgj_batch8130_marker = "libcgj-batch8130"
 *
 * Exclusive continuum CREATE-ONLY (8121-8130: strncpy pad stubs —
 * pad_byte_id, pads_with_nul_id, n_zero_ok, needs_pad, pad_count,
 * copy_len, trunc_no_nul, full_pad, pad_ok, batch_id_8130).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8130_marker[] = "libcgj-batch8130";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8130_id(void)
{
	return 8130u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8130 - report this TU's graph batch number.
 *
 * Always returns 8130. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_8130(void)
{
	(void)NULL;
	return b8130_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8130(void)
    __attribute__((alias("gj_batch_id_8130")));
