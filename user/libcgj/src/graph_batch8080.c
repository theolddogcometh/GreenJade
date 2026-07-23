/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8080: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8080(void);
 *     - Returns the compile-time graph batch number for this TU (8080).
 *   uint32_t __gj_batch_id_8080  (alias)
 *   __libcgj_batch8080_marker = "libcgj-batch8080"
 *
 * Exclusive continuum CREATE-ONLY (8071-8080: strlen max stubs —
 * size_max_id, ssize_max_id, empty_len_id, len_is_zero, len_is_max,
 * len_below_max, len_ok, len_clamp, len_errorish, batch_id_8080).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8080_marker[] = "libcgj-batch8080";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8080_id(void)
{
	return 8080u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8080 - report this TU's graph batch number.
 *
 * Always returns 8080. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_8080(void)
{
	(void)NULL;
	return b8080_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8080(void)
    __attribute__((alias("gj_batch_id_8080")));
