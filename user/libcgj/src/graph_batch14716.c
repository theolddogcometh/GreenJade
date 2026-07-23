/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14716: u32 power-of-two test (wave 14725).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14716(void);
 *     - Returns the compile-time graph batch number for this TU (14716).
 *   uint32_t gj_u32_is_pow2_14716(uint32_t v);
 *     - Returns 1 if v is a non-zero power of two, else 0.
 *   uint32_t __gj_batch_id_14716      (alias)
 *   uint32_t __gj_u32_is_pow2_14716   (alias)
 *   __libcgj_batch14716_marker = "libcgj-batch14716"
 *
 * Milestone 14725 exclusive continuum CREATE-ONLY (14701-14725). Unique
 * surface only; no multi-def. Distinct from prior is_pow2 helpers without
 * this batch suffix. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14716_marker[] = "libcgj-batch14716";

#define B14716_ID  14716u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14716_id(void)
{
	return B14716_ID;
}

static uint32_t
b14716_is_pow2(uint32_t v)
{
	if (v == 0u)
		return 0u;
	return ((v & (v - 1u)) == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14716 - report this TU's graph batch number.
 *
 * Always returns 14716.
 */
uint32_t
gj_batch_id_14716(void)
{
	(void)NULL;
	return b14716_id();
}

/*
 * gj_u32_is_pow2_14716 - test whether v is a non-zero power of two.
 *
 * Returns 1u if true, 0u otherwise. Pure integer; no libc.
 */
uint32_t
gj_u32_is_pow2_14716(uint32_t v)
{
	return b14716_is_pow2(v);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14716(void)
    __attribute__((alias("gj_batch_id_14716")));

uint32_t __gj_u32_is_pow2_14716(uint32_t v)
    __attribute__((alias("gj_u32_is_pow2_14716")));
