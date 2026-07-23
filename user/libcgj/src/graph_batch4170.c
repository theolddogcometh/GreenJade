/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4170: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4170(void);
 *     - Returns the compile-time graph batch number for this TU (4170).
 *   uint32_t __gj_batch_id_4170  (alias)
 *   __libcgj_batch4170_marker = "libcgj-batch4170"
 *
 * Exclusive continuum CREATE-ONLY (4161-4170: str_hash_fnv1a32_u3,
 * str_hash_djb2_u3, str_hash_sdbm_u3, buf_hash_fnv1a32_u2,
 * buf_hash_djb2_u2, buf_hash_sdbm_u, u32_hash_mix64_u, u64_hash_fold32_u,
 * str_len_u2, batch_id_4170). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4170_marker[] = "libcgj-batch4170";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4170_id(void)
{
	return 4170u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4170 - report this TU's graph batch number.
 *
 * Always returns 4170. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4170(void)
{
	(void)NULL;
	return b4170_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4170(void)
    __attribute__((alias("gj_batch_id_4170")));
