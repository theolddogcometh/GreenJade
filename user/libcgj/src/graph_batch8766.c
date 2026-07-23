/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8766: statx mask STATX_ATIME id soft tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_statx_atime_id_8766(void);
 *     - Returns soft STATX_ATIME bit (0x00000020).
 *   uint32_t __gj_statx_atime_id_8766  (alias)
 *   __libcgj_batch8766_marker = "libcgj-batch8766"
 *
 * Exclusive continuum CREATE-ONLY (8761-8770: statx mask id stubs).
 * Unique gj_statx_atime_id_8766 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8766_marker[] = "libcgj-batch8766";

/* Soft Linux STATX_ATIME (stx_atime present). */
#define B8766_STATX_ATIME  0x00000020u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8766_atime(void)
{
	return B8766_STATX_ATIME;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_statx_atime_id_8766 - soft STATX_ATIME request bit id.
 *
 * Always returns 0x00000020. Soft pure-data product tag for the
 * statx mask bit that asks for last-access time. Does not call
 * the statx syscall. No parent wires.
 */
uint32_t
gj_statx_atime_id_8766(void)
{
	(void)NULL;
	return b8766_atime();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_statx_atime_id_8766(void)
    __attribute__((alias("gj_statx_atime_id_8766")));
