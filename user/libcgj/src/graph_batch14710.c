/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14710: u32 min/max extract (wave 14725).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14710(void);
 *     - Returns the compile-time graph batch number for this TU (14710).
 *   uint32_t gj_u32_min_14710(uint32_t a, uint32_t b);
 *     - Returns the lesser of a and b.
 *   uint32_t gj_u32_max_14710(uint32_t a, uint32_t b);
 *     - Returns the greater of a and b.
 *   uint32_t __gj_batch_id_14710  (alias)
 *   uint32_t __gj_u32_min_14710   (alias)
 *   uint32_t __gj_u32_max_14710   (alias)
 *   __libcgj_batch14710_marker = "libcgj-batch14710"
 *
 * Milestone 14725 exclusive continuum CREATE-ONLY (14701-14725). Unique
 * surface only; no multi-def. Distinct from prior gj_u32_min / gj_u32_max
 * symbols without this batch suffix. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14710_marker[] = "libcgj-batch14710";

#define B14710_ID  14710u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14710_id(void)
{
	return B14710_ID;
}

static uint32_t
b14710_min(uint32_t a, uint32_t b)
{
	return (a < b) ? a : b;
}

static uint32_t
b14710_max(uint32_t a, uint32_t b)
{
	return (a > b) ? a : b;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14710 - report this TU's graph batch number.
 *
 * Always returns 14710.
 */
uint32_t
gj_batch_id_14710(void)
{
	(void)NULL;
	return b14710_id();
}

/*
 * gj_u32_min_14710 - lesser of two uint32 values.
 */
uint32_t
gj_u32_min_14710(uint32_t a, uint32_t b)
{
	return b14710_min(a, b);
}

/*
 * gj_u32_max_14710 - greater of two uint32 values.
 */
uint32_t
gj_u32_max_14710(uint32_t a, uint32_t b)
{
	return b14710_max(a, b);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14710(void)
    __attribute__((alias("gj_batch_id_14710")));

uint32_t __gj_u32_min_14710(uint32_t a, uint32_t b)
    __attribute__((alias("gj_u32_min_14710")));

uint32_t __gj_u32_max_14710(uint32_t a, uint32_t b)
    __attribute__((alias("gj_u32_max_14710")));
