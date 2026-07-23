/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6881: statx mask STATX_TYPE soft tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_statx_mask_type_6881(void);
 *     - Returns soft STATX_TYPE bit (0x00000001).
 *   uint32_t __gj_statx_mask_type_6881  (alias)
 *   __libcgj_batch6881_marker = "libcgj-batch6881"
 *
 * Exclusive continuum CREATE-ONLY (6881-6890: statx mask stubs —
 * type_6881, mode_6882, nlink_6883, uid_gid_6884, times_6885,
 * ino_size_blocks_6886, basic_stats_6887, has_all_6888, all/all_ok_6889,
 * continuum + batch_id_6890). Unique gj_statx_mask_type_6881 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6881_marker[] = "libcgj-batch6881";

/* Soft Linux STATX_TYPE (stx_mode S_IFMT class present). */
#define B6881_STATX_TYPE  0x00000001u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6881_type(void)
{
	return B6881_STATX_TYPE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_statx_mask_type_6881 - soft STATX_TYPE request bit.
 *
 * Always returns 0x00000001. Soft pure-data product tag for the
 * statx mask bit that asks for file type in stx_mode. Does not call
 * the statx syscall. No parent wires.
 */
uint32_t
gj_statx_mask_type_6881(void)
{
	(void)NULL;
	return b6881_type();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_statx_mask_type_6881(void)
    __attribute__((alias("gj_statx_mask_type_6881")));
