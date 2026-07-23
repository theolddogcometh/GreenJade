/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8248: big-endian host identity tag (negated).
 *
 * Surface (unique symbols):
 *   uint32_t gj_endian_be_id_8248(void);
 *     - Returns 0: host is NOT tagged big-endian for this exclusive
 *       byteswap / endian continuum (LE host assumption).
 *   uint32_t __gj_endian_be_id_8248  (alias)
 *   __libcgj_batch8248_marker = "libcgj-batch8248"
 *
 * Exclusive continuum CREATE-ONLY (8241-8250: byteswap / endian helpers).
 * Soft compile-time host-endian tag; complementary to
 * gj_endian_le_id_8247 (batch8247 returns 1). Unique
 * gj_endian_be_id_8248 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8248_marker[] = "libcgj-batch8248";

/* Host big-endian identity lamp (0 under LE host assumption). */
#define B8248_ENDIAN_BE_ID  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8248_be_id(void)
{
	return B8248_ENDIAN_BE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_endian_be_id_8248 - big-endian host identity tag.
 *
 * Always returns 0 under the continuum LE-host assumption. Soft pure
 * data product tag; does not probe memory layout at runtime. No
 * parent wires.
 */
uint32_t
gj_endian_be_id_8248(void)
{
	(void)NULL;
	return b8248_be_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_endian_be_id_8248(void)
    __attribute__((alias("gj_endian_be_id_8248")));
