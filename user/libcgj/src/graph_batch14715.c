/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14715: u32 floor average (wave 14725).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14715(void);
 *     - Returns the compile-time graph batch number for this TU (14715).
 *   uint32_t gj_u32_avg_14715(uint32_t a, uint32_t b);
 *     - Returns floor((a + b) / 2) without intermediate overflow.
 *   uint32_t __gj_batch_id_14715  (alias)
 *   uint32_t __gj_u32_avg_14715   (alias)
 *   __libcgj_batch14715_marker = "libcgj-batch14715"
 *
 * Milestone 14725 exclusive continuum CREATE-ONLY (14701-14725). Unique
 * surface only; no multi-def. Distinct from prior avg helpers without
 * this batch suffix. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14715_marker[] = "libcgj-batch14715";

#define B14715_ID  14715u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14715_id(void)
{
	return B14715_ID;
}

/* Overflow-safe floor average: (a & b) + ((a ^ b) >> 1). */
static uint32_t
b14715_avg(uint32_t a, uint32_t b)
{
	return (a & b) + ((a ^ b) >> 1);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14715 - report this TU's graph batch number.
 *
 * Always returns 14715.
 */
uint32_t
gj_batch_id_14715(void)
{
	(void)NULL;
	return b14715_id();
}

/*
 * gj_u32_avg_14715 - floor average of two uint32 values.
 *
 * Overflow-safe. Pure integer; no libc.
 */
uint32_t
gj_u32_avg_14715(uint32_t a, uint32_t b)
{
	return b14715_avg(a, b);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14715(void)
    __attribute__((alias("gj_batch_id_14715")));

uint32_t __gj_u32_avg_14715(uint32_t a, uint32_t b)
    __attribute__((alias("gj_u32_avg_14715")));
