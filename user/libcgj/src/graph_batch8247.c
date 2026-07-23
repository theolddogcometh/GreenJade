/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8247: little-endian host identity tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_endian_le_id_8247(void);
 *     - Returns 1: host is tagged little-endian for this exclusive
 *       byteswap / endian continuum (LE host assumption).
 *   uint32_t __gj_endian_le_id_8247  (alias)
 *   __libcgj_batch8247_marker = "libcgj-batch8247"
 *
 * Exclusive continuum CREATE-ONLY (8241-8250: byteswap / endian helpers).
 * Soft compile-time host-endian tag; no runtime probe. Unique
 * gj_endian_le_id_8247 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8247_marker[] = "libcgj-batch8247";

/* Host little-endian identity lamp (LE assumed for this continuum). */
#define B8247_ENDIAN_LE_ID  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8247_le_id(void)
{
	return B8247_ENDIAN_LE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_endian_le_id_8247 - little-endian host identity tag.
 *
 * Always returns 1 under the continuum LE-host assumption. Soft pure
 * data product tag; does not probe memory layout at runtime. No
 * parent wires.
 */
uint32_t
gj_endian_le_id_8247(void)
{
	(void)NULL;
	return b8247_le_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_endian_le_id_8247(void)
    __attribute__((alias("gj_endian_le_id_8247")));
