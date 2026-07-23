/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4730: batch identity for wave 4721-4730.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4730(void);
 *     - Returns the compile-time graph batch number for this TU (4730).
 *   uint32_t __gj_batch_id_4730  (alias)
 *   __libcgj_batch4730_marker = "libcgj-batch4730"
 *
 * Exclusive continuum CREATE-ONLY (4721-4730: deg_to_turns_u,
 * turns_to_deg_u, clamp_angle360_u, angle_diff_u, sin_turns_q15_u,
 * cos_turns_q15_u, isqrt_u32_floor_u, isqrt_u64_floor_u, ihypot_u32_u,
 * batch_id_4730). Unique gj_batch_id_4730 surface only; no multi-def.
 * Does NOT redefine gj_batch_id / prior batch_id_* symbols. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4730_marker[] = "libcgj-batch4730";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4730_id(void)
{
	return 4730u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4730 - report this TU's graph batch number.
 *
 * Always returns 4730 (terminal id of the 4721-4730 fp-free angle/trig
 * continuum wave). Self-contained; no parent wires.
 */
uint32_t
gj_batch_id_4730(void)
{
	(void)NULL;
	return b4730_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4730(void)
    __attribute__((alias("gj_batch_id_4730")));
