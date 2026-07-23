/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5977: Proton pfx path length soft-ok.
 *
 * Surface (unique symbols):
 *   uint32_t gj_proton_rt_pfx_path_len_ok_5977(uint32_t path_len);
 *     - Returns 1 if path_len is a plausible length for a Proton/Wine
 *       "pfx" prefix path (non-zero, <= 4096, and at least soft min 3
 *       for a "pfx" component), else 0.
 *   uint32_t __gj_proton_rt_pfx_path_len_ok_5977  (alias)
 *   __libcgj_batch5977_marker = "libcgj-batch5977"
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

const char __libcgj_batch5977_marker[] = "libcgj-batch5977";

#define B5977_PATH_MAX   4096u
#define B5977_PFX_MIN    3u   /* strlen("pfx") soft floor */

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5977_pfx_path_len_ok(uint32_t u32Len)
{
	return (u32Len >= B5977_PFX_MIN && u32Len <= B5977_PATH_MAX)
	    ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_proton_rt_pfx_path_len_ok_5977 - 1 if pfx path length is plausible.
 *
 * path_len: byte length of a Proton prefix (".../pfx") path candidate.
 * Soft length bounds only; does not parse path components.
 * No parent wires.
 */
uint32_t
gj_proton_rt_pfx_path_len_ok_5977(uint32_t u32PathLen)
{
	(void)NULL;
	return b5977_pfx_path_len_ok(u32PathLen);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_proton_rt_pfx_path_len_ok_5977(uint32_t u32PathLen)
    __attribute__((alias("gj_proton_rt_pfx_path_len_ok_5977")));
