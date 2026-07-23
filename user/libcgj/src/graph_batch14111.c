/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14111: Proton/Wine soft path max length bound.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pw_soft_path_max_14111(void);
 *     - Returns the soft max path byte length for Proton/Wine soft-path
 *       stubs (4096, PATH_MAX-style product bound).
 *   uint32_t __gj_pw_soft_path_max_14111  (alias)
 *   __libcgj_batch14111_marker = "libcgj-batch14111"
 *
 * Exclusive continuum CREATE-ONLY (14111-14120: proton/wine soft path —
 * soft_path_max_14111, soft_path_len_ok_14112, soft_path_min_ok_14113,
 * wine_bottle_path_len_ok_14114, proton_files_path_len_ok_14115,
 * soft_path_depth_ok_14116, dosdevices_path_len_ok_14117,
 * drive_c_path_len_ok_14118, soft_path_hash_mix_14119,
 * soft_path_ready_14120). Unique surface only; no multi-def. Distinct
 * from proton_rt_path_max_5971. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14111_marker[] = "libcgj-batch14111";

/* Soft PATH_MAX-style bound for Proton/Wine soft-path product stubs. */
#define B14111_PATH_MAX  4096u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14111_path_max(void)
{
	return B14111_PATH_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pw_soft_path_max_14111 - report Proton/Wine soft path max length.
 *
 * Always returns 4096. Soft compile-time bound for freestanding
 * Proton/Wine path length checks; does not touch the filesystem.
 * No parent wires.
 */
uint32_t
gj_pw_soft_path_max_14111(void)
{
	(void)NULL;
	return b14111_path_max();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pw_soft_path_max_14111(void)
    __attribute__((alias("gj_pw_soft_path_max_14111")));
