/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5974: Proton runtime path hash seed.
 *
 * Surface (unique symbols):
 *   uint32_t gj_proton_rt_path_hash_seed_5974(void);
 *     - Returns the FNV-1a 32-bit offset basis mixed with batch id,
 *       used as the seed for freestanding Proton runtime path hashes.
 *   uint32_t __gj_proton_rt_path_hash_seed_5974  (alias)
 *   __libcgj_batch5974_marker = "libcgj-batch5974"
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

const char __libcgj_batch5974_marker[] = "libcgj-batch5974";

#define B5974_BATCH_ID  5974u
#define B5974_FNV_OFF   2166136261u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5974_hash_seed(void)
{
	return B5974_FNV_OFF ^ B5974_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_proton_rt_path_hash_seed_5974 - FNV-ish seed for path hash stubs.
 *
 * Always returns (FNV offset basis XOR 5974). Soft compile-time seed
 * for freestanding Proton runtime path fingerprinting. No parent wires.
 */
uint32_t
gj_proton_rt_path_hash_seed_5974(void)
{
	(void)NULL;
	return b5974_hash_seed();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_proton_rt_path_hash_seed_5974(void)
    __attribute__((alias("gj_proton_rt_path_hash_seed_5974")));
