/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6469: screenshot size soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sshot_size_ok_6469(uint32_t width, uint32_t height,
 *                                  uint32_t byte_est);
 *     - Soft size gate: width in [1, 7680], height in [1, 4320], and
 *       byte_est in [1, 0x10000000] (256 MiB cap). All must pass → 1.
 *   uint32_t __gj_sshot_size_ok_6469  (alias)
 *   __libcgj_batch6469_marker = "libcgj-batch6469"
 *
 * Exclusive continuum CREATE-ONLY (6461-6470: screenshot path/size stubs —
 * path_len_ok_6461, path_cap_ok_6462, path_depth_ok_6463, ext_ok_6464,
 * width_ok_6465, height_ok_6466, bpp_ok_6467, byte_est_6468, size_ok_6469,
 * path_size_ready_6470). Unique surface only; no multi-def. Self-
 * contained; does not call sibling helpers. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6469_marker[] = "libcgj-batch6469";

#define B6469_W_MAX     7680u
#define B6469_H_MAX     4320u
#define B6469_BYTE_MAX  0x10000000u  /* 256 MiB soft cap */

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6469_size_ok(uint32_t u32W, uint32_t u32H, uint32_t u32Bytes)
{
	if (u32W == 0u || u32W > B6469_W_MAX) {
		return 0u;
	}
	if (u32H == 0u || u32H > B6469_H_MAX) {
		return 0u;
	}
	if (u32Bytes == 0u || u32Bytes > B6469_BYTE_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sshot_size_ok_6469 - soft screenshot dimension/byte-size gate.
 *
 * width:    soft width in pixels
 * height:   soft height in pixels
 * byte_est: soft raw/estimated byte size
 *
 * Returns 1 when all three soft bounds pass, else 0. Self-contained;
 * does not call sibling helpers. No parent wires.
 */
uint32_t
gj_sshot_size_ok_6469(uint32_t width, uint32_t height, uint32_t byte_est)
{
	(void)NULL;
	return b6469_size_ok(width, height, byte_est);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sshot_size_ok_6469(uint32_t width, uint32_t height,
    uint32_t byte_est)
    __attribute__((alias("gj_sshot_size_ok_6469")));
