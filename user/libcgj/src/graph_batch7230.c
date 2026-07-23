/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7230: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7230(void);
 *     - Returns the compile-time graph batch number for this TU (7230).
 *   uint32_t __gj_batch_id_7230  (alias)
 *   __libcgj_batch7230_marker = "libcgj-batch7230"
 *
 * Exclusive continuum CREATE-ONLY (7221-7230: select fd_set bit stubs —
 * fd_setsize, fd_nfdbits, fd_word_index, fd_bit_mask, fd_set_bit,
 * fd_clr_bit, fd_isset, fd_zero_word, fd_in_setsize, batch_id_7230).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7230_marker[] = "libcgj-batch7230";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7230_id(void)
{
	return 7230u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7230 - report this TU's graph batch number.
 *
 * Always returns 7230. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_7230(void)
{
	(void)NULL;
	return b7230_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_7230(void)
    __attribute__((alias("gj_batch_id_7230")));
