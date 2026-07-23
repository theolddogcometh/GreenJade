/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14116: Proton/Wine soft path depth gate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pw_soft_path_depth_ok_14116(uint32_t depth);
 *     - Soft path segment-depth gate: ok when depth in [1, 32]
 *       inclusive. Outside range → 0, else 1. Pure bounds, no FS.
 *   uint32_t __gj_pw_soft_path_depth_ok_14116  (alias)
 *   __libcgj_batch14116_marker = "libcgj-batch14116"
 *
 * Exclusive continuum CREATE-ONLY (14111-14120: proton/wine soft path —
 * soft_path_max_14111, soft_path_len_ok_14112, soft_path_min_ok_14113,
 * wine_bottle_path_len_ok_14114, proton_files_path_len_ok_14115,
 * soft_path_depth_ok_14116, dosdevices_path_len_ok_14117,
 * drive_c_path_len_ok_14118, soft_path_hash_mix_14119,
 * soft_path_ready_14120). Unique surface only; no multi-def. Distinct
 * from sshot_path_depth_ok_6463. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14116_marker[] = "libcgj-batch14116";

#define B14116_DEPTH_MIN  1u
#define B14116_DEPTH_MAX  32u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14116_depth_ok(uint32_t u32Depth)
{
	if (u32Depth < B14116_DEPTH_MIN || u32Depth > B14116_DEPTH_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pw_soft_path_depth_ok_14116 - soft Proton/Wine path depth gate.
 *
 * depth: soft segment count of the intended Proton/Wine path
 * Returns 1 when depth is in [1, 32], else 0. Pure integer bounds.
 * No parent wires.
 */
uint32_t
gj_pw_soft_path_depth_ok_14116(uint32_t u32Depth)
{
	(void)NULL;
	return b14116_depth_ok(u32Depth);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pw_soft_path_depth_ok_14116(uint32_t u32Depth)
    __attribute__((alias("gj_pw_soft_path_depth_ok_14116")));
