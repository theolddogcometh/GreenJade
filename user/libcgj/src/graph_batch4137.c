/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4137: store uint32_t big-endian to bytes (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_store_be_u(uint8_t *p, uint32_t v);
 *     - Write v as four big-endian octets at p (p[0] is MSB).
 *       p may be NULL → returns 0 (no write). On success returns 4.
 *       Unaligned-safe pure byte stores.
 *   uint32_t __gj_u32_store_be_u  (alias)
 *   __libcgj_batch4137_marker = "libcgj-batch4137"
 *
 * Exclusive continuum CREATE-ONLY (4131-4140: u16_load_be_u, u16_load_le_u,
 * u32_load_be_u, u32_load_le_u, u64_load_be_u, u64_load_le_u,
 * u32_store_be_u, u32_store_le_u, u16_bswap_u, batch_id_4140).
 * Distinct from internal b58/b69 store helpers — unique gj_u32_store_be_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4137_marker[] = "libcgj-batch4137";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4137_store_be(uint8_t *p, uint32_t v)
{
	if (p == NULL) {
		return 0u;
	}
	p[0] = (uint8_t)(v >> 24);
	p[1] = (uint8_t)(v >> 16);
	p[2] = (uint8_t)(v >> 8);
	p[3] = (uint8_t)v;
	return 4u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_store_be_u - store uint32_t as big-endian bytes at p.
 *
 * p: destination of at least 4 octets, or NULL
 * v: value to encode (MSB first)
 *
 * Returns 4 on success, 0 when p is NULL (bytes written).
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_store_be_u(uint8_t *p, uint32_t v)
{
	(void)NULL;
	return b4137_store_be(p, v);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_store_be_u(uint8_t *p, uint32_t v)
    __attribute__((alias("gj_u32_store_be_u")));
