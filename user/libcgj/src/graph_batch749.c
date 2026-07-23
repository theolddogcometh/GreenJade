/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch749: little-endian uint16 load from two octets.
 *
 * Surface (unique symbols):
 *   uint16_t gj_le16_load(const unsigned char *p);
 *     — Load 16-bit little-endian value from p[0..1]. NULL → 0.
 *   uint16_t __gj_le16_load  (alias)
 *   __libcgj_batch749_marker = "libcgj-batch749"
 *
 * Does NOT redefine gj_be16_load (batch747) / gj_be32_load (batch748) —
 * avoid multi-def. Unaligned-safe octet walk only.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch749_marker[] = "libcgj-batch749";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_le16_load — little-endian load of a 16-bit unsigned value.
 *
 * p: pointer to at least 2 octets, or NULL (returns 0).
 */
uint16_t
gj_le16_load(const unsigned char *p)
{
	if (p == NULL) {
		return 0u;
	}
	return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_le16_load(const unsigned char *p)
    __attribute__((alias("gj_le16_load")));
