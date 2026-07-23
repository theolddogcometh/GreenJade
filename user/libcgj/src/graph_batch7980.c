/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7980: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7980(void);
 *     - Returns the compile-time graph batch number for this TU (7980).
 *   uint32_t __gj_batch_id_7980  (alias)
 *   __libcgj_batch7980_marker = "libcgj-batch7980"
 *
 * Exclusive continuum CREATE-ONLY (7971-7980: rand/srand seed stubs —
 * rand_max_id, srand_default_id, lcg_mult_id, lcg_incr_id, seed_is_zero,
 * seed_is_default, seed_ok, seed_lcg_step, seed_errorish, batch_id_7980).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7980_marker[] = "libcgj-batch7980";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7980_id(void)
{
	return 7980u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7980 - report this TU's graph batch number.
 *
 * Always returns 7980. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_7980(void)
{
	(void)NULL;
	return b7980_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_7980(void)
    __attribute__((alias("gj_batch_id_7980")));
