/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14114: Wine bottle soft path length ok.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pw_wine_bottle_path_len_ok_14114(uint32_t path_len);
 *     - Returns 1 if path_len is a plausible length for a Wine bottle
 *       / prefix soft path (non-zero, <= 4096, and at least soft min 4
 *       for a "wine" component), else 0.
 *   uint32_t __gj_pw_wine_bottle_path_len_ok_14114  (alias)
 *   __libcgj_batch14114_marker = "libcgj-batch14114"
 *
 * Exclusive continuum CREATE-ONLY (14111-14120: proton/wine soft path —
 * soft_path_max_14111, soft_path_len_ok_14112, soft_path_min_ok_14113,
 * wine_bottle_path_len_ok_14114, proton_files_path_len_ok_14115,
 * soft_path_depth_ok_14116, dosdevices_path_len_ok_14117,
 * drive_c_path_len_ok_14118, soft_path_hash_mix_14119,
 * soft_path_ready_14120). Unique surface only; no multi-def. Distinct
 * from wine_prefix_ok_u / proton_rt_pfx_path_len_ok_5977. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14114_marker[] = "libcgj-batch14114";

#define B14114_PATH_MAX    4096u
#define B14114_BOTTLE_MIN  4u   /* strlen("wine") soft floor */

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14114_bottle_path_len_ok(uint32_t u32Len)
{
	return (u32Len >= B14114_BOTTLE_MIN && u32Len <= B14114_PATH_MAX)
	    ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pw_wine_bottle_path_len_ok_14114 - 1 if wine bottle path len ok.
 *
 * path_len: byte length of a Wine bottle / prefix path candidate.
 * Soft length bounds only; does not parse path components.
 * No parent wires.
 */
uint32_t
gj_pw_wine_bottle_path_len_ok_14114(uint32_t u32PathLen)
{
	(void)NULL;
	return b14114_bottle_path_len_ok(u32PathLen);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pw_wine_bottle_path_len_ok_14114(uint32_t u32PathLen)
    __attribute__((alias("gj_pw_wine_bottle_path_len_ok_14114")));
