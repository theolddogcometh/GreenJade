/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4368: gj_adler32_update_byte_u - Adler-32 byte.
 *
 * Surface (unique symbols):
 *   uint32_t gj_adler32_update_byte_u(uint32_t adler, uint8_t byte);
 *     - Fold one byte into a running Adler-32 packed as (s2 << 16) | s1
 *       with BASE 65521. Start with 1 for a new stream.
 *   uint32_t __gj_adler32_update_byte_u  (alias)
 *   __libcgj_batch4368_marker = "libcgj-batch4368"
 *
 * Exclusive continuum CREATE-ONLY (4361-4370). Distinct from
 * gj_adler32_update_u (batch2653 buffer fold) and gj_adler32_update
 * (batch269) — unique gj_adler32_update_byte_u surface only; no multi-def
 * of the existing buffer API. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4368_marker[] = "libcgj-batch4368";

/* Adler-32 prime modulus. */
#define B4368_BASE 65521u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4368_step(uint32_t u32Adler, uint8_t u8Byte)
{
	uint32_t u32S1;
	uint32_t u32S2;

	u32S1 = u32Adler & 0xffffu;
	u32S2 = (u32Adler >> 16) & 0xffffu;
	u32S1 += (uint32_t)u8Byte;
	if (u32S1 >= B4368_BASE) {
		u32S1 -= B4368_BASE;
	}
	u32S2 += u32S1;
	if (u32S2 >= B4368_BASE) {
		u32S2 -= B4368_BASE;
	}
	return (u32S2 << 16) | u32S1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_adler32_update_byte_u - fold one byte into a running Adler-32 state.
 *
 * adler: packed (s2 << 16) | s1 (init 1 for a new stream)
 * byte:  input byte
 *
 * Returns the updated packed state. Named _byte_u to stay exclusive of
 * gj_adler32_update_u buffer API (batch2653). Self-contained; no parent
 * wires.
 */
uint32_t
gj_adler32_update_byte_u(uint32_t u32Adler, uint8_t u8Byte)
{
	(void)NULL;
	return b4368_step(u32Adler, u8Byte);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_adler32_update_byte_u(uint32_t u32Adler, uint8_t u8Byte)
    __attribute__((alias("gj_adler32_update_byte_u")));
