/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8768: statx mask STATX_CTIME id soft tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_statx_ctime_id_8768(void);
 *     - Returns soft STATX_CTIME bit (0x00000080).
 *   uint32_t __gj_statx_ctime_id_8768  (alias)
 *   __libcgj_batch8768_marker = "libcgj-batch8768"
 *
 * Exclusive continuum CREATE-ONLY (8761-8770: statx mask id stubs).
 * Unique gj_statx_ctime_id_8768 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8768_marker[] = "libcgj-batch8768";

/* Soft Linux STATX_CTIME (stx_ctime present). */
#define B8768_STATX_CTIME  0x00000080u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8768_ctime(void)
{
	return B8768_STATX_CTIME;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_statx_ctime_id_8768 - soft STATX_CTIME request bit id.
 *
 * Always returns 0x00000080. Soft pure-data product tag for the
 * statx mask bit that asks for last-status-change time. Does not
 * call the statx syscall. No parent wires.
 */
uint32_t
gj_statx_ctime_id_8768(void)
{
	(void)NULL;
	return b8768_ctime();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_statx_ctime_id_8768(void)
    __attribute__((alias("gj_statx_ctime_id_8768")));
