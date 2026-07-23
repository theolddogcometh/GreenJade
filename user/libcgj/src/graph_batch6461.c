/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6461: screenshot path length soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sshot_path_len_ok_6461(uint32_t path_len);
 *     - Soft path length gate: ok when len in [1, 4096] inclusive.
 *       Outside range → 0, else 1. Pure bounds, no FS.
 *   uint32_t __gj_sshot_path_len_ok_6461  (alias)
 *   __libcgj_batch6461_marker = "libcgj-batch6461"
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

const char __libcgj_batch6461_marker[] = "libcgj-batch6461";

#define B6461_PATH_MIN  1u
#define B6461_PATH_MAX  4096u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6461_path_len_ok(uint32_t u32Len)
{
	if (u32Len < B6461_PATH_MIN || u32Len > B6461_PATH_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sshot_path_len_ok_6461 - soft screenshot path length gate.
 *
 * path_len: soft character/byte length of the intended screenshot path
 *
 * Returns 1 when len is in [1, 4096], else 0. Pure integer bounds.
 * No parent wires.
 */
uint32_t
gj_sshot_path_len_ok_6461(uint32_t path_len)
{
	(void)NULL;
	return b6461_path_len_ok(path_len);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sshot_path_len_ok_6461(uint32_t path_len)
    __attribute__((alias("gj_sshot_path_len_ok_6461")));
