/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8460: hex encode/decode stubs batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8460(void);
 *     - Returns the compile-time graph batch number for this TU (8460).
 *   uint32_t __gj_batch_id_8460  (alias)
 *   __libcgj_batch8460_marker = "libcgj-batch8460"
 *
 * Exclusive continuum CREATE-ONLY (8451-8460: hex encode/decode stubs —
 * digit, nibble, byte_hi, byte_lo, encode_need, decode_need, is_digit,
 * upper_id, lower_id, batch_id). Unique gj_batch_id_8460 surface only;
 * does NOT redefine bare gj_batch_id. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8460_marker[] = "libcgj-batch8460";

#define B8460_BATCH_ID  8460u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8460_id(void)
{
	return B8460_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8460 - report this TU's graph batch number.
 *
 * Always returns 8460. Soft continuum lamp for the 8451-8460 hex
 * encode/decode stubs wave. No parent wires.
 */
uint32_t
gj_batch_id_8460(void)
{
	(void)NULL;
	return b8460_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8460(void)
    __attribute__((alias("gj_batch_id_8460")));
