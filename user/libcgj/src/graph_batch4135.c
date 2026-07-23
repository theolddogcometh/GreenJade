/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4135: load uint64_t big-endian from bytes (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_load_be_u(const uint8_t *p);
 *     - Read eight octets at p as big-endian uint64_t (p[0] is MSB).
 *       p may be NULL → returns 0. Unaligned-safe pure byte loads.
 *       No __int128.
 *   uint64_t __gj_u64_load_be_u  (alias)
 *   __libcgj_batch4135_marker = "libcgj-batch4135"
 *
 * Exclusive continuum CREATE-ONLY (4131-4140: u16_load_be_u, u16_load_le_u,
 * u32_load_be_u, u32_load_le_u, u64_load_be_u, u64_load_le_u,
 * u32_store_be_u, u32_store_le_u, u16_bswap_u, batch_id_4140).
 * Distinct from internal b69 load helpers — unique gj_u64_load_be_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4135_marker[] = "libcgj-batch4135";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4135_load_be(const uint8_t *p)
{
	if (p == NULL) {
		return 0ull;
	}
	return ((uint64_t)p[0] << 56) | ((uint64_t)p[1] << 48) |
	       ((uint64_t)p[2] << 40) | ((uint64_t)p[3] << 32) |
	       ((uint64_t)p[4] << 24) | ((uint64_t)p[5] << 16) |
	       ((uint64_t)p[6] << 8) | (uint64_t)p[7];
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_load_be_u - load big-endian uint64_t from eight bytes at p.
 *
 * p: pointer to at least 8 octets, or NULL
 *
 * Returns network-order decode of p[0..7], or 0 when p is NULL.
 * Self-contained; no parent wires. No __int128.
 */
uint64_t
gj_u64_load_be_u(const uint8_t *p)
{
	(void)NULL;
	return b4135_load_be(p);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_load_be_u(const uint8_t *p)
    __attribute__((alias("gj_u64_load_be_u")));
