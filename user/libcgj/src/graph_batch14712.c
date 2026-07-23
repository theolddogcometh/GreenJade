/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14712: u32 absolute difference (wave 14725).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14712(void);
 *     - Returns the compile-time graph batch number for this TU (14712).
 *   uint32_t gj_u32_absdiff_14712(uint32_t a, uint32_t b);
 *     - Returns |a - b| as uint32_t without signed intermediate.
 *   uint32_t __gj_batch_id_14712     (alias)
 *   uint32_t __gj_u32_absdiff_14712  (alias)
 *   __libcgj_batch14712_marker = "libcgj-batch14712"
 *
 * Milestone 14725 exclusive continuum CREATE-ONLY (14701-14725). Unique
 * surface only; no multi-def. Distinct from prior absdiff helpers without
 * this batch suffix. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14712_marker[] = "libcgj-batch14712";

#define B14712_ID  14712u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14712_id(void)
{
	return B14712_ID;
}

static uint32_t
b14712_absdiff(uint32_t a, uint32_t b)
{
	return (a >= b) ? (a - b) : (b - a);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14712 - report this TU's graph batch number.
 *
 * Always returns 14712.
 */
uint32_t
gj_batch_id_14712(void)
{
	(void)NULL;
	return b14712_id();
}

/*
 * gj_u32_absdiff_14712 - absolute difference of two uint32 values.
 *
 * Pure integer; no libc. No overflow in the subtraction of the larger
 * minus the smaller.
 */
uint32_t
gj_u32_absdiff_14712(uint32_t a, uint32_t b)
{
	return b14712_absdiff(a, b);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14712(void)
    __attribute__((alias("gj_batch_id_14712")));

uint32_t __gj_u32_absdiff_14712(uint32_t a, uint32_t b)
    __attribute__((alias("gj_u32_absdiff_14712")));
