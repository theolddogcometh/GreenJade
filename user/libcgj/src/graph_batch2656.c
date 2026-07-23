/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2656: gj_fletcher16_final_u - Fletcher-16 final.
 *
 * Surface (unique symbols):
 *   uint16_t gj_fletcher16_final_u(uint32_t state);
 *     - Finalize a running Fletcher-16 packed state into the classic
 *       16-bit value (sum2 << 8) | sum1 with both sums in 0..254.
 *   uint16_t __gj_fletcher16_final_u  (alias)
 *   __libcgj_batch2656_marker = "libcgj-batch2656"
 *
 * Companion to gj_fletcher16_update_u (batch2655). Distinct from one-shot
 * gj_fletcher16 (batch183). Streaming checksum wave (2651-2660).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2656_marker[] = "libcgj-batch2656";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2656_mod255(uint32_t u32X)
{
	u32X = (u32X & 0xffu) + (u32X >> 8);
	u32X = (u32X & 0xffu) + (u32X >> 8);
	if (u32X >= 255u) {
		u32X -= 255u;
	}
	return u32X;
}

static uint16_t
b2656_final(uint32_t u32State)
{
	uint32_t u32Sum1;
	uint32_t u32Sum2;

	u32Sum1 = b2656_mod255(u32State & 0xffffu);
	u32Sum2 = b2656_mod255((u32State >> 16) & 0xffffu);
	return (uint16_t)((u32Sum2 << 8) | u32Sum1);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fletcher16_final_u - finalize a streaming Fletcher-16.
 *
 * state: packed (sum2 << 16) | sum1 from gj_fletcher16_update_u
 *        (or 0 if empty → result 0)
 *
 * Returns the 16-bit Fletcher-16 value (sum2 << 8) | sum1.
 */
uint16_t
gj_fletcher16_final_u(uint32_t u32State)
{
	(void)NULL;
	return b2656_final(u32State);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_fletcher16_final_u(uint32_t u32State)
    __attribute__((alias("gj_fletcher16_final_u")));
