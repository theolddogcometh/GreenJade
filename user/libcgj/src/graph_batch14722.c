/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14722: fixed-span memory zero (wave 14725).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14722(void);
 *     - Returns the compile-time graph batch number for this TU (14722).
 *   void gj_mem_zero_n_14722(void *p, size_t n);
 *     - Writes n zero bytes at p. No-op if p is NULL or n is 0.
 *   uint32_t __gj_batch_id_14722    (alias)
 *   void     __gj_mem_zero_n_14722  (alias)
 *   __libcgj_batch14722_marker = "libcgj-batch14722"
 *
 * Milestone 14725 exclusive continuum CREATE-ONLY (14701-14725). Unique
 * surface only; no multi-def. Distinct from prior mem_zero helpers without
 * this batch suffix. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14722_marker[] = "libcgj-batch14722";

#define B14722_ID  14722u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14722_id(void)
{
	return B14722_ID;
}

static void
b14722_mem_zero_n(void *p, size_t n)
{
	uint8_t *pb;
	size_t i;

	if (p == NULL || n == 0u)
		return;
	pb = (uint8_t *)p;
	for (i = 0; i < n; i++)
		pb[i] = 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14722 - report this TU's graph batch number.
 *
 * Always returns 14722.
 */
uint32_t
gj_batch_id_14722(void)
{
	(void)NULL;
	return b14722_id();
}

/*
 * gj_mem_zero_n_14722 - write n zero bytes at p.
 *
 * No-op on NULL or n == 0. Pure integer; no libc.
 */
void
gj_mem_zero_n_14722(void *p, size_t n)
{
	b14722_mem_zero_n(p, n);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14722(void)
    __attribute__((alias("gj_batch_id_14722")));

void __gj_mem_zero_n_14722(void *p, size_t n)
    __attribute__((alias("gj_mem_zero_n_14722")));
