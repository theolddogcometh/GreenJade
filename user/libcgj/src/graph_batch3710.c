/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3710: batch identity for exclusive decimal /
 * integer-root continuum wave (3701-3710).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3710(void);
 *     - Returns the compile-time graph batch number for this TU (3710).
 *   uint32_t __gj_batch_id_3710  (alias)
 *   __libcgj_batch3710_marker = "libcgj-batch3710"
 *
 * CREATE-ONLY exclusive continuum wave (3701-3710: u32_sqrt_ceil_u,
 * u64_sqrt_ceil_u, u32_log10_floor_u, u64_log10_floor_u,
 * u32_digit_count_u, u64_digit_count_u, u32_from_dec_digits_u,
 * u64_from_dec_digits_u, u32_to_dec_need_u, batch_id_3710). Unique
 * gj_batch_id_3710 surface only; no multi-def. Does NOT redefine
 * gj_batch_id / prior batch_id_* symbols. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3710_marker[] = "libcgj-batch3710";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3710_id(void)
{
	return 3710u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3710 - report this TU's graph batch number.
 *
 * Always returns 3710. Link-time presence tags the 3701-3710 wave.
 * No parent wires.
 */
uint32_t
gj_batch_id_3710(void)
{
	(void)NULL;
	return b3710_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3710(void)
    __attribute__((alias("gj_batch_id_3710")));
