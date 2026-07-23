/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8765: statx mask STATX_GID id soft tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_statx_gid_id_8765(void);
 *     - Returns soft STATX_GID bit (0x00000010).
 *   uint32_t __gj_statx_gid_id_8765  (alias)
 *   __libcgj_batch8765_marker = "libcgj-batch8765"
 *
 * Exclusive continuum CREATE-ONLY (8761-8770: statx mask id stubs).
 * Unique gj_statx_gid_id_8765 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8765_marker[] = "libcgj-batch8765";

/* Soft Linux STATX_GID (stx_gid present). */
#define B8765_STATX_GID  0x00000010u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8765_gid(void)
{
	return B8765_STATX_GID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_statx_gid_id_8765 - soft STATX_GID request bit id.
 *
 * Always returns 0x00000010. Soft pure-data product tag for the
 * statx mask bit that asks for owner gid. Does not call statx.
 * No parent wires.
 */
uint32_t
gj_statx_gid_id_8765(void)
{
	(void)NULL;
	return b8765_gid();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_statx_gid_id_8765(void)
    __attribute__((alias("gj_statx_gid_id_8765")));
