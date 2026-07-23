/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14720: little-endian u32 load/store (wave 14725).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14720(void);
 *     - Returns the compile-time graph batch number for this TU (14720).
 *   uint32_t gj_le32_load_14720(const uint8_t p[4]);
 *     - Loads a little-endian uint32 from four bytes. Returns 0 if p is NULL.
 *   void gj_le32_store_14720(uint8_t p[4], uint32_t v);
 *     - Stores v as little-endian into four bytes. No-op if p is NULL.
 *   uint32_t __gj_batch_id_14720   (alias)
 *   uint32_t __gj_le32_load_14720  (alias)
 *   void     __gj_le32_store_14720 (alias)
 *   __libcgj_batch14720_marker = "libcgj-batch14720"
 *
 * Milestone 14725 exclusive continuum CREATE-ONLY (14701-14725). Unique
 * surface only; no multi-def. Distinct from prior le32 helpers without
 * this batch suffix. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14720_marker[] = "libcgj-batch14720";

#define B14720_ID  14720u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14720_id(void)
{
	return B14720_ID;
}

static uint32_t
b14720_le32_load(const uint8_t *p)
{
	if (p == NULL)
		return 0u;
	return ((uint32_t)p[0]) |
	       ((uint32_t)p[1] << 8)  |
	       ((uint32_t)p[2] << 16) |
	       ((uint32_t)p[3] << 24);
}

static void
b14720_le32_store(uint8_t *p, uint32_t v)
{
	if (p == NULL)
		return;
	p[0] = (uint8_t)(v & 0xffu);
	p[1] = (uint8_t)((v >> 8) & 0xffu);
	p[2] = (uint8_t)((v >> 16) & 0xffu);
	p[3] = (uint8_t)((v >> 24) & 0xffu);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14720 - report this TU's graph batch number.
 *
 * Always returns 14720.
 */
uint32_t
gj_batch_id_14720(void)
{
	(void)NULL;
	return b14720_id();
}

/*
 * gj_le32_load_14720 - load little-endian uint32 from four bytes.
 */
uint32_t
gj_le32_load_14720(const uint8_t *p)
{
	return b14720_le32_load(p);
}

/*
 * gj_le32_store_14720 - store uint32 as little-endian into four bytes.
 */
void
gj_le32_store_14720(uint8_t *p, uint32_t v)
{
	b14720_le32_store(p, v);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14720(void)
    __attribute__((alias("gj_batch_id_14720")));

uint32_t __gj_le32_load_14720(const uint8_t *p)
    __attribute__((alias("gj_le32_load_14720")));

void __gj_le32_store_14720(uint8_t *p, uint32_t v)
    __attribute__((alias("gj_le32_store_14720")));
