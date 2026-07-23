/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4340: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4340(void);
 *     - Returns the compile-time graph batch number for this TU (4340).
 *   uint32_t __gj_batch_id_4340  (alias)
 *   __libcgj_batch4340_marker = "libcgj-batch4340"
 *
 * Exclusive continuum CREATE-ONLY (4331-4340: u32_pack_be_bytes_u,
 * u32_pack_le_bytes_u, u32_byte0_u, u32_byte1_u, u32_byte2_u, u32_byte3_u,
 * u16_pack_be_u, u16_pack_le_u, u8_nibbles_u, batch_id_4340).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4340_marker[] = "libcgj-batch4340";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4340_id(void)
{
	return 4340u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4340 - report this TU's graph batch number.
 *
 * Always returns 4340. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4340(void)
{
	(void)NULL;
	return b4340_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4340(void)
    __attribute__((alias("gj_batch_id_4340")));
