/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6365: cloud save path length-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cloud_save_path_len_ok_6365(uint32_t path_len,
 *                                          uint32_t max_len);
 *     - Returns 1 if path_len <= max_len, else 0. Soft size gate for
 *       cloud save object path bytes. max_len 0 rejects all nonzero
 *       path_len (and accepts path_len 0).
 *   uint32_t __gj_cloud_save_path_len_ok_6365  (alias)
 *   __libcgj_batch6365_marker = "libcgj-batch6365"
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

const char __libcgj_batch6365_marker[] = "libcgj-batch6365";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6365_len_ok(uint32_t u32Len, uint32_t u32Max)
{
	return (u32Len <= u32Max) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cloud_save_path_len_ok_6365 - 1 if path_len is within max_len.
 *
 * path_len: candidate path length in bytes (excluding NUL)
 * max_len:  soft maximum (e.g. from max_len_6361)
 *
 * Soft pure-data compare only. No parent wires.
 */
uint32_t
gj_cloud_save_path_len_ok_6365(uint32_t path_len, uint32_t max_len)
{
	(void)NULL;
	return b6365_len_ok(path_len, max_len);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cloud_save_path_len_ok_6365(uint32_t path_len, uint32_t max_len)
    __attribute__((alias("gj_cloud_save_path_len_ok_6365")));
