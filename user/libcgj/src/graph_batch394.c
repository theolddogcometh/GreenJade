/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch394: big-endian 32/64-bit unaligned load/store.
 *
 * Surface (unique symbols):
 *   uint32_t gj_load_be32(const uint8_t *p);
 *     — Load a 32-bit big-endian value from p[0..3]. NULL → 0.
 *   void     gj_store_be32(uint8_t *p, uint32_t v);
 *     — Store v as 32-bit big-endian into p[0..3]. NULL → no-op.
 *   uint64_t gj_load_be64(const uint8_t *p);
 *     — Load a 64-bit big-endian value from p[0..7]. NULL → 0.
 *   void     gj_store_be64(uint8_t *p, uint64_t v);
 *     — Store v as 64-bit big-endian into p[0..7]. NULL → no-op.
 *   uint32_t __gj_load_be32   (alias)
 *   void     __gj_store_be32  (alias)
 *   uint64_t __gj_load_be64   (alias)
 *   void     __gj_store_be64  (alias)
 *   __libcgj_batch394_marker = "libcgj-batch394"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. Byte-wise access is unaligned-safe. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch394_marker[] = "libcgj-batch394";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_load_be32 — big-endian uint32 load from four octets.
 *
 * p: pointer to at least 4 readable bytes (NULL → 0).
 */
uint32_t
gj_load_be32(const uint8_t *p)
{
	if (p == NULL) {
		return 0u;
	}
	return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
	       ((uint32_t)p[2] << 8) | (uint32_t)p[3];
}

/*
 * gj_store_be32 — big-endian uint32 store into four octets.
 *
 * p: pointer to at least 4 writable bytes (NULL → no-op)
 * v: value to store (MSB first)
 */
void
gj_store_be32(uint8_t *p, uint32_t v)
{
	if (p == NULL) {
		return;
	}
	p[0] = (uint8_t)(v >> 24);
	p[1] = (uint8_t)(v >> 16);
	p[2] = (uint8_t)(v >> 8);
	p[3] = (uint8_t)v;
}

/*
 * gj_load_be64 — big-endian uint64 load from eight octets.
 *
 * p: pointer to at least 8 readable bytes (NULL → 0).
 */
uint64_t
gj_load_be64(const uint8_t *p)
{
	if (p == NULL) {
		return 0u;
	}
	return ((uint64_t)p[0] << 56) | ((uint64_t)p[1] << 48) |
	       ((uint64_t)p[2] << 40) | ((uint64_t)p[3] << 32) |
	       ((uint64_t)p[4] << 24) | ((uint64_t)p[5] << 16) |
	       ((uint64_t)p[6] << 8) | (uint64_t)p[7];
}

/*
 * gj_store_be64 — big-endian uint64 store into eight octets.
 *
 * p: pointer to at least 8 writable bytes (NULL → no-op)
 * v: value to store (MSB first)
 */
void
gj_store_be64(uint8_t *p, uint64_t v)
{
	if (p == NULL) {
		return;
	}
	p[0] = (uint8_t)(v >> 56);
	p[1] = (uint8_t)(v >> 48);
	p[2] = (uint8_t)(v >> 40);
	p[3] = (uint8_t)(v >> 32);
	p[4] = (uint8_t)(v >> 24);
	p[5] = (uint8_t)(v >> 16);
	p[6] = (uint8_t)(v >> 8);
	p[7] = (uint8_t)v;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_load_be32(const uint8_t *p)
    __attribute__((alias("gj_load_be32")));

void __gj_store_be32(uint8_t *p, uint32_t v)
    __attribute__((alias("gj_store_be32")));

uint64_t __gj_load_be64(const uint8_t *p)
    __attribute__((alias("gj_load_be64")));

void __gj_store_be64(uint8_t *p, uint64_t v)
    __attribute__((alias("gj_store_be64")));
