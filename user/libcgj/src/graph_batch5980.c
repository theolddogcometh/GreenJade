/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5980: Proton runtime path/hash wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5980(void);
 *     - Returns the compile-time graph batch number for this TU (5980).
 *   uint32_t __gj_batch_id_5980  (alias)
 *   __libcgj_batch5980_marker = "libcgj-batch5980"
 *
 * Exclusive continuum CREATE-ONLY (5971-5980: Proton runtime path
 * length/hash stubs —
 * proton_rt_path_max_5971, proton_rt_path_len_ok_5972,
 * proton_rt_path_min_ok_5973, proton_rt_path_hash_seed_5974,
 * proton_rt_path_hash_len_5975, proton_rt_path_hash_mix_5976,
 * proton_rt_pfx_path_len_ok_5977, proton_rt_compat_path_len_ok_5978,
 * proton_rt_path_hash_ready_5979, batch_id_5980). Unique surface only;
 * no multi-def. Does NOT redefine gj_batch_id / prior batch_id_*.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5980_marker[] = "libcgj-batch5980";

#define B5980_BATCH_ID  5980u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5980_id(void)
{
	return B5980_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5980 - report this TU's graph batch number.
 *
 * Always returns 5980 (Proton runtime path length/hash wave closer).
 * No parent wires.
 */
uint32_t
gj_batch_id_5980(void)
{
	(void)NULL;
	return b5980_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_5980(void)
    __attribute__((alias("gj_batch_id_5980")));
