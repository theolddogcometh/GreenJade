/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5971: Proton runtime path max length bound.
 *
 * Surface (unique symbols):
 *   uint32_t gj_proton_rt_path_max_5971(void);
 *     - Returns the soft max path byte length for Proton runtime path
 *       stubs (4096, PATH_MAX-style product bound).
 *   uint32_t __gj_proton_rt_path_max_5971  (alias)
 *   __libcgj_batch5971_marker = "libcgj-batch5971"
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

const char __libcgj_batch5971_marker[] = "libcgj-batch5971";

/* Soft PATH_MAX-style bound for Proton runtime path product stubs. */
#define B5971_PATH_MAX  4096u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5971_path_max(void)
{
	return B5971_PATH_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_proton_rt_path_max_5971 - report Proton runtime path max length.
 *
 * Always returns 4096. Soft compile-time bound for freestanding Proton
 * runtime path length checks; does not touch the filesystem.
 * No parent wires.
 */
uint32_t
gj_proton_rt_path_max_5971(void)
{
	(void)NULL;
	return b5971_path_max();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_proton_rt_path_max_5971(void)
    __attribute__((alias("gj_proton_rt_path_max_5971")));
