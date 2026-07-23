/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3730: batch identity for exclusive RNG
 * continuum wave (3721-3730).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3730(void);
 *     - Returns the compile-time graph batch number for this TU (3730).
 *   uint32_t __gj_batch_id_3730  (alias)
 *   __libcgj_batch3730_marker = "libcgj-batch3730"
 *
 * CREATE-ONLY exclusive continuum wave (3721-3730: rng_xorshift32_u,
 * rng_xorshift64_u, rng_splitmix64_u, rng_pcg32_u, rng_uniform_u32_u,
 * rng_uniform_u64_u, rng_range_u32_u, rng_range_u64_u, rng_bool_u,
 * batch_id_3730). Unique gj_batch_id_3730 surface only; no multi-def.
 * Does NOT redefine gj_batch_id / prior batch_id_* symbols. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3730_marker[] = "libcgj-batch3730";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3730_id(void)
{
	return 3730u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3730 - report this TU's graph batch number.
 *
 * Always returns 3730. Link-time presence tags the 3721-3730 RNG wave.
 * No parent wires.
 */
uint32_t
gj_batch_id_3730(void)
{
	(void)NULL;
	return b3730_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3730(void)
    __attribute__((alias("gj_batch_id_3730")));
