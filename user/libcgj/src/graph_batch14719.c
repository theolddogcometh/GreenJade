/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14719: big-endian u32 load/store (wave 14725).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14719(void);
 *     - Returns the compile-time graph batch number for this TU (14719).
 *   uint32_t gj_be32_load_14719(const uint8_t p[4]);
 *     - Loads a big-endian uint32 from four bytes. Returns 0 if p is NULL.
 *   void gj_be32_store_14719(uint8_t p[4], uint32_t v);
 *     - Stores v as big-endian into four bytes. No-op if p is NULL.
 *   uint32_t __gj_batch_id_14719   (alias)
 *   uint32_t __gj_be32_load_14719  (alias)
 *   void     __gj_be32_store_14719 (alias)
 *   __libcgj_batch14719_marker = "libcgj-batch14719"
 *
 * Milestone 14725 exclusive continuum CREATE-ONLY (14701-14725). Unique
 * surface only; no multi-def. Distinct from prior be32 helpers without
 * this batch suffix. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14719_marker[] = "libcgj-batch14719";

#define B14719_ID  14719u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14719_id(void)
{
	return B14719_ID;
}

static uint32_t
b14719_be32_load(const uint8_t *p)
{
	if (p == NULL)
		return 0u;
	return ((uint32_t)p[0] << 24) |
	       ((uint32_t)p[1] << 16) |
	       ((uint32_t)p[2] << 8)  |
	       ((uint32_t)p[3]);
}

static void
b14719_be32_store(uint8_t *p, uint32_t v)
{
	if (p == NULL)
		return;
	p[0] = (uint8_t)((v >> 24) & 0xffu);
	p[1] = (uint8_t)((v >> 16) & 0xffu);
	p[2] = (uint8_t)((v >> 8) & 0xffu);
	p[3] = (uint8_t)(v & 0xffu);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14719 - report this TU's graph batch number.
 *
 * Always returns 14719.
 */
uint32_t
gj_batch_id_14719(void)
{
	(void)NULL;
	return b14719_id();
}

/*
 * gj_be32_load_14719 - load big-endian uint32 from four bytes.
 */
uint32_t
gj_be32_load_14719(const uint8_t *p)
{
	return b14719_be32_load(p);
}

/*
 * gj_be32_store_14719 - store uint32 as big-endian into four bytes.
 */
void
gj_be32_store_14719(uint8_t *p, uint32_t v)
{
	b14719_be32_store(p, v);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14719(void)
    __attribute__((alias("gj_batch_id_14719")));

uint32_t __gj_be32_load_14719(const uint8_t *p)
    __attribute__((alias("gj_be32_load_14719")));

void __gj_be32_store_14719(uint8_t *p, uint32_t v)
    __attribute__((alias("gj_be32_store_14719")));
