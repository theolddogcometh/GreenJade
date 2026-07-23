/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14724: FNV-1a 32-bit buffer hash (wave 14725).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14724(void);
 *     - Returns the compile-time graph batch number for this TU (14724).
 *   uint32_t gj_fnv1a32_14724(const void *p, size_t n);
 *     - Returns the FNV-1a 32-bit hash of n bytes at p. Empty / NULL
 *       span returns the FNV offset basis (0x811c9dc5).
 *   uint32_t __gj_batch_id_14724   (alias)
 *   uint32_t __gj_fnv1a32_14724    (alias)
 *   __libcgj_batch14724_marker = "libcgj-batch14724"
 *
 * Milestone 14725 exclusive continuum CREATE-ONLY (14701-14725). Unique
 * surface only; no multi-def. Distinct from prior fnv1a helpers without
 * this batch suffix. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14724_marker[] = "libcgj-batch14724";

#define B14724_ID           14724u
#define B14724_FNV_OFFSET   0x811c9dc5u
#define B14724_FNV_PRIME    0x01000193u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14724_id(void)
{
	return B14724_ID;
}

static uint32_t
b14724_fnv1a32(const void *p, size_t n)
{
	const uint8_t *pb;
	uint32_t h;
	size_t i;

	h = B14724_FNV_OFFSET;
	if (p == NULL || n == 0u)
		return h;
	pb = (const uint8_t *)p;
	for (i = 0; i < n; i++) {
		h ^= (uint32_t)pb[i];
		h *= B14724_FNV_PRIME;
	}
	return h;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14724 - report this TU's graph batch number.
 *
 * Always returns 14724.
 */
uint32_t
gj_batch_id_14724(void)
{
	(void)NULL;
	return b14724_id();
}

/*
 * gj_fnv1a32_14724 - FNV-1a 32-bit hash over a fixed span.
 *
 * NULL or empty returns the FNV offset basis. Pure integer; no libc.
 */
uint32_t
gj_fnv1a32_14724(const void *p, size_t n)
{
	return b14724_fnv1a32(p, n);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14724(void)
    __attribute__((alias("gj_batch_id_14724")));

uint32_t __gj_fnv1a32_14724(const void *p, size_t n)
    __attribute__((alias("gj_fnv1a32_14724")));
