/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4030: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4030(void);
 *     - Returns the compile-time graph batch number for this TU (4030).
 *   uint32_t __gj_batch_id_4030  (alias)
 *   __libcgj_batch4030_marker = "libcgj-batch4030"
 *
 * Exclusive continuum CREATE-ONLY (4021-4030: str_hash_crc32c_u,
 * str_hash_adler_u, buf_hash_crc32c_u, buf_hash_adler_u, u32_mix_final_u,
 * u64_mix_final_u, u32_avalanche_u, u64_avalanche_u, u32_scramble_u,
 * batch_id_4030). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4030_marker[] = "libcgj-batch4030";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4030_id(void)
{
	return 4030u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4030 - report this TU's graph batch number.
 *
 * Always returns 4030. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_4030(void)
{
	(void)NULL;
	return b4030_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4030(void)
    __attribute__((alias("gj_batch_id_4030")));
