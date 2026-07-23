/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6466: screenshot height soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sshot_height_ok_6466(uint32_t height);
 *     - Soft height gate: ok when height in [1, 4320] inclusive
 *       (up to 8K vertical). Outside range → 0, else 1.
 *   uint32_t __gj_sshot_height_ok_6466  (alias)
 *   __libcgj_batch6466_marker = "libcgj-batch6466"
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

const char __libcgj_batch6466_marker[] = "libcgj-batch6466";

#define B6466_H_MIN  1u
#define B6466_H_MAX  4320u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6466_height_ok(uint32_t u32H)
{
	if (u32H < B6466_H_MIN || u32H > B6466_H_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sshot_height_ok_6466 - soft screenshot pixel-height gate.
 *
 * height: soft image height in pixels
 *
 * Returns 1 when height is in [1, 4320], else 0. Pure integer bounds.
 * No parent wires.
 */
uint32_t
gj_sshot_height_ok_6466(uint32_t height)
{
	(void)NULL;
	return b6466_height_ok(height);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sshot_height_ok_6466(uint32_t height)
    __attribute__((alias("gj_sshot_height_ok_6466")));
