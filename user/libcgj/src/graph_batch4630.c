/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4630: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4630(void);
 *     - Returns the compile-time graph batch number for this TU (4630).
 *   uint32_t __gj_batch_id_4630  (alias)
 *   __libcgj_batch4630_marker = "libcgj-batch4630"
 *
 * Exclusive continuum CREATE-ONLY (4621-4630: entropy_mix32_u,
 * entropy_mix64_u, entropy_combine32_u, entropy_combine64_u,
 * entropy_rot_mix_u, entropy_avalanche32_u, entropy_avalanche64_u,
 * entropy_from_bytes_u, entropy_seed_seq_u, batch_id_4630).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4630_marker[] = "libcgj-batch4630";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4630_id(void)
{
	return 4630u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4630 - report this TU's graph batch number.
 *
 * Always returns 4630. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4630(void)
{
	(void)NULL;
	return b4630_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4630(void)
    __attribute__((alias("gj_batch_id_4630")));
