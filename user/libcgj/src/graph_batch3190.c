/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3190: MILESTONE 3190 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3190(void);
 *     - Returns the compile-time graph batch number for this TU (3190).
 *   uint32_t __gj_batch_id_3190  (alias)
 *   __libcgj_batch3190_marker = "libcgj-batch3190"
 *
 * Milestone for the exclusive continuum CREATE-ONLY wave
 * (batches 3181-3189: str_hash_fnv1a32_u2, str_hash_djb2_u2,
 * str_hash_sdbm_u2, str_hash_murmur_mix_u, buf_eq_u, buf_is_sorted_u8_u,
 * buf_sum_u8_u, buf_xor_u8_u, buf_max_u8_u). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols - avoid
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3190_marker[] = "libcgj-batch3190";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3190_id(void)
{
	return 3190u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3190 - report this TU's graph batch number.
 *
 * Always returns 3190. Link-time presence of this symbol tags the wave.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_batch_id_3190(void)
{
	(void)NULL;
	return b3190_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3190(void)
    __attribute__((alias("gj_batch_id_3190")));
