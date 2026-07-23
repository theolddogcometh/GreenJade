/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14721: fixed-span memory equality (wave 14725).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14721(void);
 *     - Returns the compile-time graph batch number for this TU (14721).
 *   uint32_t gj_mem_eq_n_14721(const void *a, const void *b, size_t n);
 *     - Returns 1 if the n-byte spans at a and b are equal, else 0.
 *       Returns 1 if n is 0. Returns 0 if either pointer is NULL and n > 0.
 *   uint32_t __gj_batch_id_14721   (alias)
 *   uint32_t __gj_mem_eq_n_14721   (alias)
 *   __libcgj_batch14721_marker = "libcgj-batch14721"
 *
 * Milestone 14725 exclusive continuum CREATE-ONLY (14701-14725). Unique
 * surface only; no multi-def. Distinct from prior mem_eq helpers without
 * this batch suffix. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14721_marker[] = "libcgj-batch14721";

#define B14721_ID  14721u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14721_id(void)
{
	return B14721_ID;
}

static uint32_t
b14721_mem_eq_n(const void *a, const void *b, size_t n)
{
	const uint8_t *pa;
	const uint8_t *pb;
	size_t i;

	if (n == 0u)
		return 1u;
	if (a == NULL || b == NULL)
		return 0u;
	if (a == b)
		return 1u;

	pa = (const uint8_t *)a;
	pb = (const uint8_t *)b;
	for (i = 0; i < n; i++) {
		if (pa[i] != pb[i])
			return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14721 - report this TU's graph batch number.
 *
 * Always returns 14721.
 */
uint32_t
gj_batch_id_14721(void)
{
	(void)NULL;
	return b14721_id();
}

/*
 * gj_mem_eq_n_14721 - compare two fixed-length byte spans.
 *
 * Returns 1u on equality, 0u otherwise. Pure integer; no libc.
 */
uint32_t
gj_mem_eq_n_14721(const void *a, const void *b, size_t n)
{
	return b14721_mem_eq_n(a, b, n);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14721(void)
    __attribute__((alias("gj_batch_id_14721")));

uint32_t __gj_mem_eq_n_14721(const void *a, const void *b, size_t n)
    __attribute__((alias("gj_mem_eq_n_14721")));
