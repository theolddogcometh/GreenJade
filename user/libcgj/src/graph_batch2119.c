/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2119: gj_crc64_ecma_poly - ECMA-182 poly constant.
 *
 * Surface (unique symbols):
 *   uint64_t gj_crc64_ecma_poly(void);
 *     - Returns the non-reflected CRC-64/ECMA-182 polynomial constant
 *       0x42F0E1EBA9EA3693ull (catalog poly; refin=false form).
 *   uint64_t __gj_crc64_ecma_poly  (alias)
 *   __libcgj_batch2119_marker = "libcgj-batch2119"
 *
 * Post-2100 crc exclusive wave. Constant-only surface. Does NOT redefine
 * gj_crc64_ecma / crc64_ecma one-shot implementations (batch268 / batch41).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2119_marker[] = "libcgj-batch2119";

/* CRC-64/ECMA-182 non-reflected polynomial. */
#define B2119_CRC64_ECMA_POLY 0x42F0E1EBA9EA3693ull

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b2119_poly_ecma(void)
{
	return B2119_CRC64_ECMA_POLY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc64_ecma_poly - CRC-64/ECMA-182 polynomial constant.
 *
 * Always returns 0x42F0E1EBA9EA3693ull.
 */
uint64_t
gj_crc64_ecma_poly(void)
{
	(void)NULL;
	return b2119_poly_ecma();
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_crc64_ecma_poly(void)
    __attribute__((alias("gj_crc64_ecma_poly")));
