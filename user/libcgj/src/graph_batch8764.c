/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8764: statx mask STATX_UID id soft tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_statx_uid_id_8764(void);
 *     - Returns soft STATX_UID bit (0x00000008).
 *   uint32_t __gj_statx_uid_id_8764  (alias)
 *   __libcgj_batch8764_marker = "libcgj-batch8764"
 *
 * Exclusive continuum CREATE-ONLY (8761-8770: statx mask id stubs).
 * Unique gj_statx_uid_id_8764 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8764_marker[] = "libcgj-batch8764";

/* Soft Linux STATX_UID (stx_uid present). */
#define B8764_STATX_UID  0x00000008u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8764_uid(void)
{
	return B8764_STATX_UID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_statx_uid_id_8764 - soft STATX_UID request bit id.
 *
 * Always returns 0x00000008. Soft pure-data product tag for the
 * statx mask bit that asks for owner uid. Does not call statx.
 * No parent wires.
 */
uint32_t
gj_statx_uid_id_8764(void)
{
	(void)NULL;
	return b8764_uid();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_statx_uid_id_8764(void)
    __attribute__((alias("gj_statx_uid_id_8764")));
