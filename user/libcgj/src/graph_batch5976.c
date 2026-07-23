/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5976: Proton runtime path hash mix.
 *
 * Surface (unique symbols):
 *   uint32_t gj_proton_rt_path_hash_mix_5976(uint32_t seed,
 *                                            uint32_t path_len);
 *     - Mix seed with path_len via FNV-ish steps; product combine for
 *       Proton runtime path fingerprint pipelines.
 *   uint32_t __gj_proton_rt_path_hash_mix_5976  (alias)
 *   __libcgj_batch5976_marker = "libcgj-batch5976"
 *
 * Exclusive continuum CREATE-ONLY (5971-5980: Proton runtime path
 * length/hash stubs —
 * proton_rt_path_max_5971, proton_rt_path_len_ok_5972,
 * proton_rt_path_min_ok_5973, proton_rt_path_hash_seed_5974,
 * proton_rt_path_hash_len_5975, proton_rt_path_hash_mix_5976,
 * proton_rt_pfx_path_len_ok_5977, proton_rt_compat_path_len_ok_5978,
 * proton_rt_path_hash_ready_5979, batch_id_5980). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5976_marker[] = "libcgj-batch5976";

#define B5976_BATCH_ID   5976u
#define B5976_FNV_PRIME  16777619u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5976_hash_mix(uint32_t u32Seed, uint32_t u32Len)
{
	uint32_t u32H;
	uint32_t i;

	u32H = u32Seed ^ B5976_BATCH_ID;
	for (i = 0u; i < 4u; i++) {
		u32H ^= (u32Len >> (i * 8u)) & 0xffu;
		u32H *= B5976_FNV_PRIME;
	}
	return u32H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_proton_rt_path_hash_mix_5976 - mix seed with path length.
 *
 * seed: prior hash/seed (e.g. from path_hash_seed_5974).
 * path_len: byte length to fold in.
 * Soft combine for freestanding Proton runtime path fingerprints.
 * No parent wires.
 */
uint32_t
gj_proton_rt_path_hash_mix_5976(uint32_t u32Seed, uint32_t u32PathLen)
{
	(void)NULL;
	return b5976_hash_mix(u32Seed, u32PathLen);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_proton_rt_path_hash_mix_5976(uint32_t u32Seed,
    uint32_t u32PathLen)
    __attribute__((alias("gj_proton_rt_path_hash_mix_5976")));
