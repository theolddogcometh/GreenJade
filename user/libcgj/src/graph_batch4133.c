/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4133: load uint32_t big-endian from bytes (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_load_be_u(const uint8_t *p);
 *     - Read four octets at p as big-endian uint32_t (p[0] is MSB).
 *       p may be NULL → returns 0. Unaligned-safe pure byte loads.
 *   uint32_t __gj_u32_load_be_u  (alias)
 *   __libcgj_batch4133_marker = "libcgj-batch4133"
 *
 * Exclusive continuum CREATE-ONLY (4131-4140: u16_load_be_u, u16_load_le_u,
 * u32_load_be_u, u32_load_le_u, u64_load_be_u, u64_load_le_u,
 * u32_store_be_u, u32_store_le_u, u16_bswap_u, batch_id_4140).
 * Distinct from internal b58/b69 load helpers — unique gj_u32_load_be_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4133_marker[] = "libcgj-batch4133";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4133_load_be(const uint8_t *p)
{
	if (p == NULL) {
		return 0u;
	}
	return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
	       ((uint32_t)p[2] << 8) | (uint32_t)p[3];
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_load_be_u - load big-endian uint32_t from four bytes at p.
 *
 * p: pointer to at least 4 octets, or NULL
 *
 * Returns network-order decode of p[0..3], or 0 when p is NULL.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_load_be_u(const uint8_t *p)
{
	(void)NULL;
	return b4133_load_be(p);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_load_be_u(const uint8_t *p)
    __attribute__((alias("gj_u32_load_be_u")));
