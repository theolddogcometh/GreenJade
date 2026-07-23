/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch748: big-endian uint32 load from four octets.
 *
 * Surface (unique symbols):
 *   uint32_t gj_be32_load(const unsigned char *p);
 *     — Load 32-bit big-endian value from p[0..3]. NULL → 0.
 *   uint32_t __gj_be32_load  (alias)
 *   __libcgj_batch748_marker = "libcgj-batch748"
 *
 * Does NOT redefine gj_be16_load (batch747) or htonl/ntohl (byteorder.c)
 * — avoid multi-def. Unaligned-safe octet walk only.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch748_marker[] = "libcgj-batch748";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_be32_load — big-endian load of a 32-bit unsigned value.
 *
 * p: pointer to at least 4 octets, or NULL (returns 0).
 */
uint32_t
gj_be32_load(const unsigned char *p)
{
	if (p == NULL) {
		return 0u;
	}
	return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
	    ((uint32_t)p[2] << 8) | (uint32_t)p[3];
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_be32_load(const unsigned char *p)
    __attribute__((alias("gj_be32_load")));
