/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch395: little-endian unaligned load/store helpers.
 *
 * Surface (unique symbols):
 *   uint32_t gj_load_le32(const uint8_t *p);
 *     — Load a 32-bit little-endian value from 4 bytes at p (unaligned-safe).
 *   void     gj_store_le32(uint8_t *p, uint32_t v);
 *     — Store a 32-bit value as little-endian to 4 bytes at p.
 *   uint64_t gj_load_le64(const uint8_t *p);
 *     — Load a 64-bit little-endian value from 8 bytes at p (unaligned-safe).
 *   void     gj_store_le64(uint8_t *p, uint64_t v);
 *     — Store a 64-bit value as little-endian to 8 bytes at p.
 *   uint32_t __gj_load_le32  (alias)
 *   void     __gj_store_le32 (alias)
 *   uint64_t __gj_load_le64  (alias)
 *   void     __gj_store_le64 (alias)
 *   __libcgj_batch395_marker = "libcgj-batch395"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch395_marker[] = "libcgj-batch395";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_load_le32 — read 4 LE bytes without assuming alignment.
 */
uint32_t
gj_load_le32(const uint8_t *p)
{
	return (uint32_t)p[0] |
	       ((uint32_t)p[1] << 8) |
	       ((uint32_t)p[2] << 16) |
	       ((uint32_t)p[3] << 24);
}

/*
 * gj_store_le32 — write 4 LE bytes without assuming alignment.
 */
void
gj_store_le32(uint8_t *p, uint32_t u32V)
{
	p[0] = (uint8_t)u32V;
	p[1] = (uint8_t)(u32V >> 8);
	p[2] = (uint8_t)(u32V >> 16);
	p[3] = (uint8_t)(u32V >> 24);
}

/*
 * gj_load_le64 — read 8 LE bytes without assuming alignment.
 */
uint64_t
gj_load_le64(const uint8_t *p)
{
	return (uint64_t)p[0] |
	       ((uint64_t)p[1] << 8) |
	       ((uint64_t)p[2] << 16) |
	       ((uint64_t)p[3] << 24) |
	       ((uint64_t)p[4] << 32) |
	       ((uint64_t)p[5] << 40) |
	       ((uint64_t)p[6] << 48) |
	       ((uint64_t)p[7] << 56);
}

/*
 * gj_store_le64 — write 8 LE bytes without assuming alignment.
 */
void
gj_store_le64(uint8_t *p, uint64_t u64V)
{
	p[0] = (uint8_t)u64V;
	p[1] = (uint8_t)(u64V >> 8);
	p[2] = (uint8_t)(u64V >> 16);
	p[3] = (uint8_t)(u64V >> 24);
	p[4] = (uint8_t)(u64V >> 32);
	p[5] = (uint8_t)(u64V >> 40);
	p[6] = (uint8_t)(u64V >> 48);
	p[7] = (uint8_t)(u64V >> 56);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_load_le32(const uint8_t *p)
    __attribute__((alias("gj_load_le32")));

void __gj_store_le32(uint8_t *p, uint32_t u32V)
    __attribute__((alias("gj_store_le32")));

uint64_t __gj_load_le64(const uint8_t *p)
    __attribute__((alias("gj_load_le64")));

void __gj_store_le64(uint8_t *p, uint64_t u64V)
    __attribute__((alias("gj_store_le64")));
