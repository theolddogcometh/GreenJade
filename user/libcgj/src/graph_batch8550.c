/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8550: math classify integer stubs batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8550(void);
 *     - Returns the compile-time graph batch number for this TU (8550).
 *   uint32_t __gj_batch_id_8550  (alias)
 *   __libcgj_batch8550_marker = "libcgj-batch8550"
 *
 * Exclusive continuum CREATE-ONLY (8541-8550: math classify integer
 * stubs on IEEE binary32 bits as uint32 — signbit, exp, mant, iszero,
 * isfinite, isinf, isnan, isnormal, classify_id, batch_id). Unique
 * gj_batch_id_8550 surface only; does NOT redefine bare gj_batch_id.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8550_marker[] = "libcgj-batch8550";

#define B8550_BATCH_ID  8550u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8550_id(void)
{
	return B8550_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8550 - report this TU's graph batch number.
 *
 * Always returns 8550. Soft continuum lamp for the 8541-8550 math
 * classify integer stubs wave. No parent wires.
 */
uint32_t
gj_batch_id_8550(void)
{
	(void)NULL;
	return b8550_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8550(void)
    __attribute__((alias("gj_batch_id_8550")));
