/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4190: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4190(void);
 *     - Returns the compile-time graph batch number for this TU (4190).
 *   uint32_t __gj_batch_id_4190  (alias)
 *   __libcgj_batch4190_marker = "libcgj-batch4190"
 *
 * Exclusive continuum CREATE-ONLY (4181-4190: u8_sum_checksum_u,
 * u16_sum_checksum_u, u32_sum_checksum_u, xor_reduce8_u, xor_reduce32_u,
 * ones_complement_sum16_u, fletcher16_u, fletcher32_u, luhn_u32_u,
 * batch_id_4190).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4190_marker[] = "libcgj-batch4190";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4190_id(void)
{
	return 4190u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4190 - report this TU's graph batch number.
 *
 * Always returns 4190. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4190(void)
{
	(void)NULL;
	return b4190_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4190(void)
    __attribute__((alias("gj_batch_id_4190")));
