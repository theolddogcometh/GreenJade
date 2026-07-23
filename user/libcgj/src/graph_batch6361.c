/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6361: cloud save path max length stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cloud_save_path_max_len_6361(void);
 *     - Returns the soft max cloud save object path length in bytes
 *       excluding the terminating NUL (4096). Integer-only bound for
 *       freestanding cloud save path gates.
 *   uint32_t __gj_cloud_save_path_max_len_6361  (alias)
 *   __libcgj_batch6361_marker = "libcgj-batch6361"
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

const char __libcgj_batch6361_marker[] = "libcgj-batch6361";

/* Soft practical max path bytes for a cloud save object key (excludes NUL). */
#define B6361_PATH_MAX_LEN  4096u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6361_max_len(void)
{
	return B6361_PATH_MAX_LEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cloud_save_path_max_len_6361 - soft max cloud save path bytes.
 *
 * Always returns 4096. Soft bound only; does not query a cloud backend.
 * No parent wires.
 */
uint32_t
gj_cloud_save_path_max_len_6361(void)
{
	(void)NULL;
	return b6361_max_len();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cloud_save_path_max_len_6361(void)
    __attribute__((alias("gj_cloud_save_path_max_len_6361")));
