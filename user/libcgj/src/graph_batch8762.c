/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8762: statx mask STATX_MODE id soft tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_statx_mode_id_8762(void);
 *     - Returns soft STATX_MODE bit (0x00000002).
 *   uint32_t __gj_statx_mode_id_8762  (alias)
 *   __libcgj_batch8762_marker = "libcgj-batch8762"
 *
 * Exclusive continuum CREATE-ONLY (8761-8770: statx mask id stubs).
 * Unique gj_statx_mode_id_8762 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8762_marker[] = "libcgj-batch8762";

/* Soft Linux STATX_MODE (full stx_mode permission bits present). */
#define B8762_STATX_MODE  0x00000002u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8762_mode(void)
{
	return B8762_STATX_MODE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_statx_mode_id_8762 - soft STATX_MODE request bit id.
 *
 * Always returns 0x00000002. Soft pure-data product tag for the
 * statx mask bit that asks for full mode bits. Does not call statx.
 * No parent wires.
 */
uint32_t
gj_statx_mode_id_8762(void)
{
	(void)NULL;
	return b8762_mode();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_statx_mode_id_8762(void)
    __attribute__((alias("gj_statx_mode_id_8762")));
