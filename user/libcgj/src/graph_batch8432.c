/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8432: CRC-32 poly id integer stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_crc32_poly_id_8432(void);
 *     - Return the unreflected IEEE CRC-32 polynomial 0x04C11DB7u
 *       (MSB-first / normal form; reflected form is 0xEDB88320).
 *   uint32_t __gj_crc32_poly_id_8432  (alias)
 *   __libcgj_batch8432_marker = "libcgj-batch8432"
 *
 * Exclusive continuum CREATE-ONLY (8431-8440: crc/poly integer stubs).
 * Unique gj_crc32_poly_id_8432 surface only; no multi-def. Distinct
 * from gj_crc32_poly_ieee (batch1706, reflected 0xEDB88320). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8432_marker[] = "libcgj-batch8432";

/* CRC-32 poly unreflected (normal) form. */
#define B8432_CRC32_POLY 0x04C11DB7u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8432_crc32_poly_id(void)
{
	return B8432_CRC32_POLY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc32_poly_id_8432 - unreflected IEEE CRC-32 polynomial id.
 *
 * Always returns 0x04C11DB7u. Soft catalog only; does not run a CRC.
 * No parent wires.
 */
uint32_t
gj_crc32_poly_id_8432(void)
{
	(void)NULL;
	return b8432_crc32_poly_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_crc32_poly_id_8432(void)
    __attribute__((alias("gj_crc32_poly_id_8432")));
