/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14714: u32 saturating subtract (wave 14725).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14714(void);
 *     - Returns the compile-time graph batch number for this TU (14714).
 *   uint32_t gj_u32_sat_sub_14714(uint32_t a, uint32_t b);
 *     - Returns a - b, saturating at 0 on underflow.
 *   uint32_t __gj_batch_id_14714     (alias)
 *   uint32_t __gj_u32_sat_sub_14714  (alias)
 *   __libcgj_batch14714_marker = "libcgj-batch14714"
 *
 * Milestone 14725 exclusive continuum CREATE-ONLY (14701-14725). Unique
 * surface only; no multi-def. Distinct from prior sat_sub helpers without
 * this batch suffix. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14714_marker[] = "libcgj-batch14714";

#define B14714_ID  14714u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14714_id(void)
{
	return B14714_ID;
}

static uint32_t
b14714_sat_sub(uint32_t a, uint32_t b)
{
	if (a < b)
		return 0u;
	return a - b;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14714 - report this TU's graph batch number.
 *
 * Always returns 14714.
 */
uint32_t
gj_batch_id_14714(void)
{
	(void)NULL;
	return b14714_id();
}

/*
 * gj_u32_sat_sub_14714 - saturating unsigned 32-bit subtraction.
 *
 * Underflow clamps to 0. Pure integer; no libc.
 */
uint32_t
gj_u32_sat_sub_14714(uint32_t a, uint32_t b)
{
	return b14714_sat_sub(a, b);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14714(void)
    __attribute__((alias("gj_batch_id_14714")));

uint32_t __gj_u32_sat_sub_14714(uint32_t a, uint32_t b)
    __attribute__((alias("gj_u32_sat_sub_14714")));
