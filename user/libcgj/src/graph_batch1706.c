/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1706: IEEE CRC-32 reflected polynomial.
 *
 * Surface (unique symbols):
 *   uint32_t gj_crc32_poly_ieee(void);
 *     — Returns the bit-reflected IEEE / zlib / PNG CRC-32 polynomial
 *       constant 0xEDB88320u (unreflected form is 0x04C11DB7).
 *   uint32_t __gj_crc32_poly_ieee  (alias)
 *   __libcgj_batch1706_marker = "libcgj-batch1706"
 *
 * Constant-only surface. Does NOT redefine crc32 / crc32_z / gj_crc32
 * implementations.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1706_marker[] = "libcgj-batch1706";

/* IEEE poly 0x04C11DB7 reflected → 0xEDB88320. */
#define B1706_CRC32_POLY_IEEE_REFLECTED 0xEDB88320u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b1706_poly_ieee(void)
{
	return B1706_CRC32_POLY_IEEE_REFLECTED;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc32_poly_ieee — IEEE / zlib CRC-32 reflected polynomial constant.
 *
 * Always returns 0xEDB88320u.
 */
uint32_t
gj_crc32_poly_ieee(void)
{
	return b1706_poly_ieee();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_crc32_poly_ieee(void)
    __attribute__((alias("gj_crc32_poly_ieee")));
