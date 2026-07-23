/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8280: div/mod safe continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8280(void);
 *     - Returns the compile-time graph batch number for this TU (8280).
 *   uint32_t __gj_batch_id_8280  (alias)
 *   __libcgj_batch8280_marker = "libcgj-batch8280"
 *
 * Exclusive continuum CREATE-ONLY (8271-8280: div/mod safe integer
 * stubs — div_safe, mod_safe, div_ceil, div_floor, is_pow2, align_up,
 * align_down, round_up_pow2, gcd, batch_id_8280). Unique
 * gj_batch_id_8280 surface only; no multi-def. Does NOT redefine bare
 * gj_batch_id. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8280_marker[] = "libcgj-batch8280";

#define B8280_BATCH_ID  8280u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8280_id(void)
{
	return B8280_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8280 - report this TU's graph batch number.
 *
 * Always returns 8280. Soft pure-data product tag that the div/mod
 * safe integer stubs exclusive wave is present. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_batch_id_8280(void)
{
	(void)NULL;
	return b8280_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8280(void)
    __attribute__((alias("gj_batch_id_8280")));
