/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8150: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8150(void);
 *     - Returns the compile-time graph batch number for this TU (8150).
 *   uint32_t __gj_batch_id_8150  (alias)
 *   __libcgj_batch8150_marker = "libcgj-batch8150"
 *
 * Exclusive continuum CREATE-ONLY (8141-8150: strstr needle stubs —
 * empty_needle_ok, not_found_id, match_zero_id, needle_is_empty,
 * hay_is_empty, needle_fits, offset_ok, params_ok, needle_errorish,
 * batch_id_8150). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8150_marker[] = "libcgj-batch8150";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8150_id(void)
{
	return 8150u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8150 - report this TU's graph batch number.
 *
 * Always returns 8150. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_8150(void)
{
	(void)NULL;
	return b8150_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8150(void)
    __attribute__((alias("gj_batch_id_8150")));
