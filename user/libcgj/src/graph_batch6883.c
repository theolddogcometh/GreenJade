/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6883: statx mask STATX_NLINK soft tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_statx_mask_nlink_6883(void);
 *     - Returns soft STATX_NLINK bit (0x00000004).
 *   uint32_t __gj_statx_mask_nlink_6883  (alias)
 *   __libcgj_batch6883_marker = "libcgj-batch6883"
 *
 * Exclusive continuum CREATE-ONLY (6881-6890: statx mask stubs).
 * Unique gj_statx_mask_nlink_6883 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6883_marker[] = "libcgj-batch6883";

/* Soft Linux STATX_NLINK (stx_nlink present). */
#define B6883_STATX_NLINK  0x00000004u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6883_nlink(void)
{
	return B6883_STATX_NLINK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_statx_mask_nlink_6883 - soft STATX_NLINK request bit.
 *
 * Always returns 0x00000004. Soft pure-data product tag for the
 * statx mask bit that asks for hard-link count. Does not call statx.
 * No parent wires.
 */
uint32_t
gj_statx_mask_nlink_6883(void)
{
	(void)NULL;
	return b6883_nlink();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_statx_mask_nlink_6883(void)
    __attribute__((alias("gj_statx_mask_nlink_6883")));
