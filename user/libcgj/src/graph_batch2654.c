/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2654: gj_adler32_final_u - Adler-32 stream final.
 *
 * Surface (unique symbols):
 *   uint32_t gj_adler32_final_u(uint32_t state);
 *     - Identity finalize: Adler-32 needs no padding or residual block;
 *       the running packed state from gj_adler32_update_u is complete.
 *   uint32_t __gj_adler32_final_u  (alias)
 *   __libcgj_batch2654_marker = "libcgj-batch2654"
 *
 * Companion to gj_adler32_update_u (batch2653). Distinct from
 * gj_adler32_final (batch269) — unique _u surface. Streaming checksum
 * wave (2651-2660).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2654_marker[] = "libcgj-batch2654";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2654_identity(uint32_t u32State)
{
	return u32State;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_adler32_final_u - finalize a streaming Adler-32.
 *
 * state: packed running value from gj_adler32_update_u (or 1 if empty)
 *
 * Returns state unchanged (Adler-32 has no final mix step).
 */
uint32_t
gj_adler32_final_u(uint32_t u32State)
{
	(void)NULL;
	return b2654_identity(u32State);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_adler32_final_u(uint32_t u32State)
    __attribute__((alias("gj_adler32_final_u")));
