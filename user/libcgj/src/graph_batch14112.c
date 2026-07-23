/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14112: Proton/Wine soft path length ok gate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pw_soft_path_len_ok_14112(uint32_t path_len);
 *     - Returns 1 if path_len is non-zero and within the soft
 *       Proton/Wine path max (4096), else 0.
 *   uint32_t __gj_pw_soft_path_len_ok_14112  (alias)
 *   __libcgj_batch14112_marker = "libcgj-batch14112"
 *
 * Exclusive continuum CREATE-ONLY (14111-14120: proton/wine soft path —
 * soft_path_max_14111, soft_path_len_ok_14112, soft_path_min_ok_14113,
 * wine_bottle_path_len_ok_14114, proton_files_path_len_ok_14115,
 * soft_path_depth_ok_14116, dosdevices_path_len_ok_14117,
 * drive_c_path_len_ok_14118, soft_path_hash_mix_14119,
 * soft_path_ready_14120). Unique surface only; no multi-def. Distinct
 * from proton_rt_path_len_ok_5972. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14112_marker[] = "libcgj-batch14112";

#define B14112_PATH_MAX  4096u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14112_path_len_ok(uint32_t u32Len)
{
	return (u32Len > 0u && u32Len <= B14112_PATH_MAX) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pw_soft_path_len_ok_14112 - 1 if Proton/Wine soft path length ok.
 *
 * path_len: byte length of a Proton/Wine path string candidate.
 * Soft bounds only; does not inspect path contents. No parent wires.
 */
uint32_t
gj_pw_soft_path_len_ok_14112(uint32_t u32PathLen)
{
	(void)NULL;
	return b14112_path_len_ok(u32PathLen);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pw_soft_path_len_ok_14112(uint32_t u32PathLen)
    __attribute__((alias("gj_pw_soft_path_len_ok_14112")));
