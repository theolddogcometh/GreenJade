/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14119: Proton/Wine soft path hash mix.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pw_soft_path_hash_mix_14119(uint32_t seed,
 *                                           uint32_t path_len);
 *     - Mix seed with path_len via FNV-ish steps; product combine for
 *       Proton/Wine soft path fingerprint pipelines.
 *   uint32_t __gj_pw_soft_path_hash_mix_14119  (alias)
 *   __libcgj_batch14119_marker = "libcgj-batch14119"
 *
 * Exclusive continuum CREATE-ONLY (14111-14120: proton/wine soft path —
 * soft_path_max_14111, soft_path_len_ok_14112, soft_path_min_ok_14113,
 * wine_bottle_path_len_ok_14114, proton_files_path_len_ok_14115,
 * soft_path_depth_ok_14116, dosdevices_path_len_ok_14117,
 * drive_c_path_len_ok_14118, soft_path_hash_mix_14119,
 * soft_path_ready_14120). Unique surface only; no multi-def. Distinct
 * from proton_rt_path_hash_mix_5976. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14119_marker[] = "libcgj-batch14119";

#define B14119_BATCH_ID   14119u
#define B14119_FNV_PRIME  16777619u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14119_hash_mix(uint32_t u32Seed, uint32_t u32Len)
{
	uint32_t u32H;
	uint32_t i;

	u32H = u32Seed ^ B14119_BATCH_ID;
	for (i = 0u; i < 4u; i++) {
		u32H ^= (u32Len >> (i * 8u)) & 0xffu;
		u32H *= B14119_FNV_PRIME;
	}
	return u32H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pw_soft_path_hash_mix_14119 - mix seed with path length.
 *
 * seed: prior hash/seed for the soft path pipeline.
 * path_len: byte length to fold in.
 * Soft combine for freestanding Proton/Wine path fingerprints.
 * No parent wires.
 */
uint32_t
gj_pw_soft_path_hash_mix_14119(uint32_t u32Seed, uint32_t u32PathLen)
{
	(void)NULL;
	return b14119_hash_mix(u32Seed, u32PathLen);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pw_soft_path_hash_mix_14119(uint32_t u32Seed,
    uint32_t u32PathLen)
    __attribute__((alias("gj_pw_soft_path_hash_mix_14119")));
