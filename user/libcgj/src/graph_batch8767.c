/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8767: statx mask STATX_MTIME id soft tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_statx_mtime_id_8767(void);
 *     - Returns soft STATX_MTIME bit (0x00000040).
 *   uint32_t __gj_statx_mtime_id_8767  (alias)
 *   __libcgj_batch8767_marker = "libcgj-batch8767"
 *
 * Exclusive continuum CREATE-ONLY (8761-8770: statx mask id stubs).
 * Unique gj_statx_mtime_id_8767 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8767_marker[] = "libcgj-batch8767";

/* Soft Linux STATX_MTIME (stx_mtime present). */
#define B8767_STATX_MTIME  0x00000040u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8767_mtime(void)
{
	return B8767_STATX_MTIME;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_statx_mtime_id_8767 - soft STATX_MTIME request bit id.
 *
 * Always returns 0x00000040. Soft pure-data product tag for the
 * statx mask bit that asks for last-modification time. Does not call
 * the statx syscall. No parent wires.
 */
uint32_t
gj_statx_mtime_id_8767(void)
{
	(void)NULL;
	return b8767_mtime();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_statx_mtime_id_8767(void)
    __attribute__((alias("gj_statx_mtime_id_8767")));
