/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5973: Proton runtime path min length soft-ok.
 *
 * Surface (unique symbols):
 *   uint32_t gj_proton_rt_path_min_ok_5973(uint32_t path_len);
 *     - Returns 1 if path_len is at least the soft min absolute path
 *       length for a Proton runtime root (1 for "/" sentinel, product
 *       min is 1), else 0. Complements path_len_ok upper bound.
 *   uint32_t __gj_proton_rt_path_min_ok_5973  (alias)
 *   __libcgj_batch5973_marker = "libcgj-batch5973"
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

const char __libcgj_batch5973_marker[] = "libcgj-batch5973";

/* Soft minimum non-empty path length (at least one byte). */
#define B5973_PATH_MIN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5973_path_min_ok(uint32_t u32Len)
{
	return (u32Len >= B5973_PATH_MIN) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_proton_rt_path_min_ok_5973 - 1 if path length meets soft minimum.
 *
 * path_len: byte length of a candidate Proton runtime path.
 * Returns 1 when path_len >= 1, else 0. No parent wires.
 */
uint32_t
gj_proton_rt_path_min_ok_5973(uint32_t u32PathLen)
{
	(void)NULL;
	return b5973_path_min_ok(u32PathLen);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_proton_rt_path_min_ok_5973(uint32_t u32PathLen)
    __attribute__((alias("gj_proton_rt_path_min_ok_5973")));
