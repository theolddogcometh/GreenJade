/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7680: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7680(void);
 *     - Returns the compile-time graph batch number for this TU (7680).
 *   uint32_t __gj_batch_id_7680  (alias)
 *   __libcgj_batch7680_marker = "libcgj-batch7680"
 *
 * Exclusive continuum CREATE-ONLY (7671-7680: gettimeofday timeval stubs —
 * usec_per_sec, usec_max, usec_ok, tv_ok, tv_zero_p, tv_eq, tz_null_ok,
 * success_id, usec_norm_rem, batch_id_7680).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7680_marker[] = "libcgj-batch7680";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7680_id(void)
{
	return 7680u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7680 - report this TU's graph batch number.
 *
 * Always returns 7680. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_7680(void)
{
	(void)NULL;
	return b7680_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_7680(void)
    __attribute__((alias("gj_batch_id_7680")));
