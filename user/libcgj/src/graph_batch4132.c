/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4132: load uint16_t little-endian from bytes (_u).
 *
 * Surface (unique symbols):
 *   uint16_t gj_u16_load_le_u(const uint8_t *p);
 *     - Read two octets at p as little-endian uint16_t (p[0] is LSB).
 *       p may be NULL → returns 0. Unaligned-safe pure byte loads.
 *   uint16_t __gj_u16_load_le_u  (alias)
 *   __libcgj_batch4132_marker = "libcgj-batch4132"
 *
 * Exclusive continuum CREATE-ONLY (4131-4140: u16_load_be_u, u16_load_le_u,
 * u32_load_be_u, u32_load_le_u, u64_load_be_u, u64_load_le_u,
 * u32_store_be_u, u32_store_le_u, u16_bswap_u, batch_id_4140).
 * Distinct from internal b58/b75 load helpers — unique gj_u16_load_le_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4132_marker[] = "libcgj-batch4132";

/* ---- freestanding helpers ---------------------------------------------- */

static uint16_t
b4132_load_le(const uint8_t *p)
{
	if (p == NULL) {
		return 0u;
	}
	return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u16_load_le_u - load little-endian uint16_t from two bytes at p.
 *
 * p: pointer to at least 2 octets, or NULL
 *
 * Returns p[0]|(p[1]<<8), or 0 when p is NULL.
 * Self-contained; no parent wires.
 */
uint16_t
gj_u16_load_le_u(const uint8_t *p)
{
	(void)NULL;
	return b4132_load_le(p);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_u16_load_le_u(const uint8_t *p)
    __attribute__((alias("gj_u16_load_le_u")));
