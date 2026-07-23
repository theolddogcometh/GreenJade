/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6463: screenshot path depth soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sshot_path_depth_ok_6463(uint32_t depth);
 *     - Soft path segment-depth gate: ok when depth in [1, 32]
 *       inclusive. Outside range → 0, else 1.
 *   uint32_t __gj_sshot_path_depth_ok_6463  (alias)
 *   __libcgj_batch6463_marker = "libcgj-batch6463"
 *
 * Exclusive continuum CREATE-ONLY (6461-6470: screenshot path/size stubs —
 * path_len_ok_6461, path_cap_ok_6462, path_depth_ok_6463, ext_ok_6464,
 * width_ok_6465, height_ok_6466, bpp_ok_6467, byte_est_6468, size_ok_6469,
 * path_size_ready_6470). Unique surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6463_marker[] = "libcgj-batch6463";

#define B6463_DEPTH_MIN  1u
#define B6463_DEPTH_MAX  32u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6463_path_depth_ok(uint32_t u32Depth)
{
	if (u32Depth < B6463_DEPTH_MIN || u32Depth > B6463_DEPTH_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sshot_path_depth_ok_6463 - soft screenshot path segment-depth gate.
 *
 * depth: soft count of path segments (directories + basename)
 *
 * Returns 1 when depth is in [1, 32], else 0. Pure integer bounds.
 * No parent wires.
 */
uint32_t
gj_sshot_path_depth_ok_6463(uint32_t depth)
{
	(void)NULL;
	return b6463_path_depth_ok(depth);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sshot_path_depth_ok_6463(uint32_t depth)
    __attribute__((alias("gj_sshot_path_depth_ok_6463")));
