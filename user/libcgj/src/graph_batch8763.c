/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8763: statx mask STATX_NLINK id soft tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_statx_nlink_id_8763(void);
 *     - Returns soft STATX_NLINK bit (0x00000004).
 *   uint32_t __gj_statx_nlink_id_8763  (alias)
 *   __libcgj_batch8763_marker = "libcgj-batch8763"
 *
 * Exclusive continuum CREATE-ONLY (8761-8770: statx mask id stubs).
 * Unique gj_statx_nlink_id_8763 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8763_marker[] = "libcgj-batch8763";

/* Soft Linux STATX_NLINK (stx_nlink present). */
#define B8763_STATX_NLINK  0x00000004u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8763_nlink(void)
{
	return B8763_STATX_NLINK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_statx_nlink_id_8763 - soft STATX_NLINK request bit id.
 *
 * Always returns 0x00000004. Soft pure-data product tag for the
 * statx mask bit that asks for link count. Does not call statx.
 * No parent wires.
 */
uint32_t
gj_statx_nlink_id_8763(void)
{
	(void)NULL;
	return b8763_nlink();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_statx_nlink_id_8763(void)
    __attribute__((alias("gj_statx_nlink_id_8763")));
