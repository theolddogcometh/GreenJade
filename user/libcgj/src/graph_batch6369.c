/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6369: cloud save path pack (segs|len).
 *
 * Surface (unique symbols):
 *   uint32_t gj_cloud_save_path_pack_6369(uint32_t seg_count,
 *                                        uint32_t path_len);
 *     - Pack soft size fields into one summary word:
 *         bits  0..15 = seg_count clamped to 0..0xffff
 *         bits 16..31 = path_len  clamped to 0..0xffff
 *       Compact probe tag for the cloud save path continuum.
 *   uint32_t __gj_cloud_save_path_pack_6369  (alias)
 *   __libcgj_batch6369_marker = "libcgj-batch6369"
 *
 * Exclusive continuum CREATE-ONLY (6361-6370: cloud save path stubs —
 * max_len_6361, abs_ok_6362, seg_count_6363, depth_ok_6364,
 * len_ok_6365, leaf_len_6366, leaf_ok_6367, score_6368, pack_6369,
 * batch_id / wave_ready_6370). Unique surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6369_marker[] = "libcgj-batch6369";

#define B6369_U16_MAX  0xffffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6369_clamp16(uint32_t u32V)
{
	if (u32V > B6369_U16_MAX) {
		return B6369_U16_MAX;
	}
	return u32V;
}

static uint32_t
b6369_pack(uint32_t u32Segs, uint32_t u32Len)
{
	return b6369_clamp16(u32Segs) | (b6369_clamp16(u32Len) << 16);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cloud_save_path_pack_6369 - pack segment count + path length.
 *
 * seg_count: non-empty segment count (clamped to 16 bits)
 * path_len:  path length in bytes (clamped to 16 bits)
 *
 * Returns packed summary word. Soft pure-data only. No parent wires.
 */
uint32_t
gj_cloud_save_path_pack_6369(uint32_t seg_count, uint32_t path_len)
{
	(void)NULL;
	return b6369_pack(seg_count, path_len);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cloud_save_path_pack_6369(uint32_t seg_count, uint32_t path_len)
    __attribute__((alias("gj_cloud_save_path_pack_6369")));
