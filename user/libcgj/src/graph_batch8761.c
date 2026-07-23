/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8761: statx mask STATX_TYPE id soft tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_statx_type_id_8761(void);
 *     - Returns soft STATX_TYPE bit (0x00000001).
 *   uint32_t __gj_statx_type_id_8761  (alias)
 *   __libcgj_batch8761_marker = "libcgj-batch8761"
 *
 * Exclusive continuum CREATE-ONLY (8761-8770: statx mask id stubs —
 * type_8761, mode_8762, nlink_8763, uid_8764, gid_8765, atime_8766,
 * mtime_8767, ctime_8768, size_8769, batch_id_8770). Unique
 * gj_statx_type_id_8761 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8761_marker[] = "libcgj-batch8761";

/* Soft Linux STATX_TYPE (stx_mode S_IFMT class present). */
#define B8761_STATX_TYPE  0x00000001u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8761_type(void)
{
	return B8761_STATX_TYPE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_statx_type_id_8761 - soft STATX_TYPE request bit id.
 *
 * Always returns 0x00000001. Soft pure-data product tag for the
 * statx mask bit that asks for file type in stx_mode. Does not call
 * the statx syscall. No parent wires.
 */
uint32_t
gj_statx_type_id_8761(void)
{
	(void)NULL;
	return b8761_type();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_statx_type_id_8761(void)
    __attribute__((alias("gj_statx_type_id_8761")));
