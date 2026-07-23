/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7120: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7120(void);
 *     - Returns the compile-time graph batch number for this TU (7120).
 *   uint32_t __gj_batch_id_7120  (alias)
 *   __libcgj_batch7120_marker = "libcgj-batch7120"
 *
 * Exclusive continuum CREATE-ONLY (7111-7120: clock_gettime clock id
 * stubs — realtime_id, monotonic_id, process_cputime_id,
 * thread_cputime_id, monotonic_raw_id, boottime_id, id_ok,
 * id_is_monotonic, id_errorish, batch_id_7120).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7120_marker[] = "libcgj-batch7120";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7120_id(void)
{
	return 7120u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7120 - report this TU's graph batch number.
 *
 * Always returns 7120. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_7120(void)
{
	(void)NULL;
	return b7120_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_7120(void)
    __attribute__((alias("gj_batch_id_7120")));
