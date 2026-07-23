/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1707: CRC-32C Castagnoli polynomial constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_crc32c_poly(void);
 *     — Returns the bit-reflected Castagnoli CRC-32C polynomial
 *       constant 0x82F63B78u (unreflected form is 0x1EDC6F41).
 *   uint32_t __gj_crc32c_poly  (alias)
 *   __libcgj_batch1707_marker = "libcgj-batch1707"
 *
 * Constant-only surface. Does NOT redefine crc32c / gj_crc32c
 * implementations (batch39 / batch87).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1707_marker[] = "libcgj-batch1707";

/* Castagnoli poly 0x1EDC6F41 reflected → 0x82F63B78. */
#define B1707_CRC32C_POLY_REFLECTED 0x82F63B78u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b1707_poly_c(void)
{
	return B1707_CRC32C_POLY_REFLECTED;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc32c_poly — Castagnoli CRC-32C reflected polynomial constant.
 *
 * Always returns 0x82F63B78u.
 */
uint32_t
gj_crc32c_poly(void)
{
	return b1707_poly_c();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_crc32c_poly(void)
    __attribute__((alias("gj_crc32c_poly")));
