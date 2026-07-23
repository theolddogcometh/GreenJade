/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6465: screenshot width soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sshot_width_ok_6465(uint32_t width);
 *     - Soft width gate: ok when width in [1, 7680] inclusive
 *       (up to 8K horizontal). Outside range → 0, else 1.
 *   uint32_t __gj_sshot_width_ok_6465  (alias)
 *   __libcgj_batch6465_marker = "libcgj-batch6465"
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

const char __libcgj_batch6465_marker[] = "libcgj-batch6465";

#define B6465_W_MIN  1u
#define B6465_W_MAX  7680u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6465_width_ok(uint32_t u32W)
{
	if (u32W < B6465_W_MIN || u32W > B6465_W_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sshot_width_ok_6465 - soft screenshot pixel-width gate.
 *
 * width: soft image width in pixels
 *
 * Returns 1 when width is in [1, 7680], else 0. Pure integer bounds.
 * No parent wires.
 */
uint32_t
gj_sshot_width_ok_6465(uint32_t width)
{
	(void)NULL;
	return b6465_width_ok(width);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sshot_width_ok_6465(uint32_t width)
    __attribute__((alias("gj_sshot_width_ok_6465")));
