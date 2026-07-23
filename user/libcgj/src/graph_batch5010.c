/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5010: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5010(void);
 *     - Returns the compile-time graph batch number for this TU (5010).
 *   uint32_t __gj_batch_id_5010  (alias)
 *   __libcgj_batch5010_marker = "libcgj-batch5010"
 *
 * Exclusive continuum CREATE-ONLY (5001-5010: u32_broadcast_byte_u,
 * u64_broadcast_byte_u, u32_has_zero_byte_u, u64_has_zero_byte_u,
 * u32_has_byte_u, u32_count_bytes_eq_u, u32_first_byte_eq_u,
 * u32_last_byte_eq_u, u32_bytes_sum_u, batch_id_5010). Post-5000
 * identity/bit unique wave. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5010_marker[] = "libcgj-batch5010";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5010_id(void)
{
	return 5010u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5010 - report this TU's graph batch number.
 *
 * Always returns 5010. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_5010(void)
{
	(void)NULL;
	return b5010_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_5010(void)
    __attribute__((alias("gj_batch_id_5010")));
