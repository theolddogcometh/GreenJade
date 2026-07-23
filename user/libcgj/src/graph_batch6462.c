/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6462: screenshot path capacity soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sshot_path_cap_ok_6462(uint32_t path_len, uint32_t cap);
 *     - Soft capacity gate: ok when cap > path_len (room for NUL) and
 *       cap <= 8192. path_len must be non-zero. Else 0.
 *   uint32_t __gj_sshot_path_cap_ok_6462  (alias)
 *   __libcgj_batch6462_marker = "libcgj-batch6462"
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

const char __libcgj_batch6462_marker[] = "libcgj-batch6462";

#define B6462_CAP_MAX  8192u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6462_path_cap_ok(uint32_t u32Len, uint32_t u32Cap)
{
	if (u32Len == 0u || u32Cap == 0u) {
		return 0u;
	}
	if (u32Cap > B6462_CAP_MAX) {
		return 0u;
	}
	if (u32Cap <= u32Len) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sshot_path_cap_ok_6462 - soft screenshot path buffer capacity gate.
 *
 * path_len: soft path length (chars/bytes, excluding NUL)
 * cap:      destination buffer capacity including NUL
 *
 * Returns 1 when cap > path_len and cap <= 8192 and path_len != 0.
 * Pure integer. No parent wires.
 */
uint32_t
gj_sshot_path_cap_ok_6462(uint32_t path_len, uint32_t cap)
{
	(void)NULL;
	return b6462_path_cap_ok(path_len, cap);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sshot_path_cap_ok_6462(uint32_t path_len, uint32_t cap)
    __attribute__((alias("gj_sshot_path_cap_ok_6462")));
