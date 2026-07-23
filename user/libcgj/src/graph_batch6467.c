/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6467: screenshot bits-per-pixel soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sshot_bpp_ok_6467(uint32_t bpp);
 *     - Soft bpp gate: 8, 16, 24, or 32 → 1; else 0. Pure table.
 *   uint32_t __gj_sshot_bpp_ok_6467  (alias)
 *   __libcgj_batch6467_marker = "libcgj-batch6467"
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

const char __libcgj_batch6467_marker[] = "libcgj-batch6467";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6467_bpp_ok(uint32_t u32Bpp)
{
	if (u32Bpp == 8u || u32Bpp == 16u || u32Bpp == 24u ||
	    u32Bpp == 32u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sshot_bpp_ok_6467 - soft screenshot bits-per-pixel gate.
 *
 * bpp: soft bits per pixel (8/16/24/32 accepted)
 *
 * Returns 1 for known bpp values, else 0. Pure integer. No parent wires.
 */
uint32_t
gj_sshot_bpp_ok_6467(uint32_t bpp)
{
	(void)NULL;
	return b6467_bpp_ok(bpp);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sshot_bpp_ok_6467(uint32_t bpp)
    __attribute__((alias("gj_sshot_bpp_ok_6467")));
