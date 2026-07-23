/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2087: unaligned big-endian uint16 load (_u).
 *
 * Surface (unique symbols):
 *   uint16_t gj_load_be16_u(const uint8_t *p);
 *     - Load a 16-bit big-endian value from p[0..1]. NULL returns 0.
 *       Unaligned-safe octet walk only.
 *   uint16_t __gj_load_be16_u  (alias)
 *   __libcgj_batch2087_marker = "libcgj-batch2087"
 *
 * Endian store exclusive wave (2081-2090). Distinct from gj_be16_load
 * (batch747) and gj_load_be16_unaligned (batch1493) - unique _u name;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2087_marker[] = "libcgj-batch2087";

/* ---- freestanding helpers ---------------------------------------------- */

static uint16_t
b2087_load_be16(const uint8_t *pSrc)
{
	return (uint16_t)(((uint16_t)pSrc[0] << 8) | (uint16_t)pSrc[1]);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_load_be16_u - big-endian uint16 load (unaligned-safe).
 *
 * p: pointer to at least 2 readable octets, or NULL (returns 0)
 * Layout: p[0] is the most-significant octet.
 */
uint16_t
gj_load_be16_u(const uint8_t *p)
{
	(void)NULL;
	if (p == NULL) {
		return 0u;
	}
	return b2087_load_be16(p);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_load_be16_u(const uint8_t *p)
    __attribute__((alias("gj_load_be16_u")));
