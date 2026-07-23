/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5975: Proton runtime path-length hash fold.
 *
 * Surface (unique symbols):
 *   uint32_t gj_proton_rt_path_hash_len_5975(uint32_t path_len);
 *     - Fold path_len into a 32-bit FNV-ish fingerprint (length-only
 *       hash stub for Proton runtime path identity tables).
 *   uint32_t __gj_proton_rt_path_hash_len_5975  (alias)
 *   __libcgj_batch5975_marker = "libcgj-batch5975"
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

const char __libcgj_batch5975_marker[] = "libcgj-batch5975";

#define B5975_BATCH_ID   5975u
#define B5975_FNV_OFF    2166136261u
#define B5975_FNV_PRIME  16777619u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5975_hash_len(uint32_t u32Len)
{
	uint32_t u32H;
	uint32_t i;

	u32H = B5975_FNV_OFF ^ B5975_BATCH_ID;
	/* Fold four little-endian bytes of path_len. */
	for (i = 0u; i < 4u; i++) {
		u32H ^= (u32Len >> (i * 8u)) & 0xffu;
		u32H *= B5975_FNV_PRIME;
	}
	return u32H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_proton_rt_path_hash_len_5975 - FNV-ish hash of a path length.
 *
 * path_len: byte length to fingerprint (no string body required).
 * Soft length-only hash for Proton runtime path tables. No parent wires.
 */
uint32_t
gj_proton_rt_path_hash_len_5975(uint32_t u32PathLen)
{
	(void)NULL;
	return b5975_hash_len(u32PathLen);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_proton_rt_path_hash_len_5975(uint32_t u32PathLen)
    __attribute__((alias("gj_proton_rt_path_hash_len_5975")));
