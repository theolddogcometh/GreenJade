/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2920: milestone 2920 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2920(void);
 *     - Returns the compile-time graph batch number for this TU (2920).
 *   uint32_t __gj_batch_id_2920  (alias)
 *   __libcgj_batch2920_marker = "libcgj-batch2920"
 *
 * Milestone for the exclusive bit/pow2 continuum wave (batches 2911-2919:
 * u64_mod_pow2_u, u32/u64_round_up_pow2_u, hamming_u32/u64_u,
 * gray_encode/decode_u32_u, bitrev8/16_u). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols -
 * avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2920_marker[] = "libcgj-batch2920";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2920_id(void)
{
	return 2920u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2920 - report this TU's graph batch number.
 *
 * Always returns 2920. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_2920(void)
{
	(void)NULL;
	return b2920_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_2920(void)
    __attribute__((alias("gj_batch_id_2920")));
