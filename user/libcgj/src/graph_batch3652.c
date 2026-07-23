/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3652: gj_crc32c_final_u - CRC-32C stream final.
 *
 * Surface (unique symbols):
 *   uint32_t gj_crc32c_final_u(uint32_t state);
 *     - Finalize a running CRC-32C (Castagnoli) internal state by XOR
 *       with 0xFFFFFFFF (xorout). Empty stream with init state
 *       0xFFFFFFFF yields 0x00000000.
 *   uint32_t __gj_crc32c_final_u  (alias)
 *   __libcgj_batch3652_marker = "libcgj-batch3652"
 *
 * Companion to gj_crc32c_update_u (batch3651). Hash/checksum exclusive
 * wave (3651-3660). Does NOT redefine gj_crc32c_sw / crc32c /
 * gj_crc32_final_u — unique Castagnoli _final_u surface only.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3652_marker[] = "libcgj-batch3652";

/* CRC-32C final XOR-out (matches init complement). */
#define B3652_XOROUT 0xFFFFFFFFu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3652_final(uint32_t u32State)
{
	return u32State ^ B3652_XOROUT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc32c_final_u - finalize a streaming CRC-32C.
 *
 * state: internal CRC from gj_crc32c_update_u (or init 0xFFFFFFFF if empty)
 *
 * Returns the finalized 32-bit CRC-32C (state XOR 0xFFFFFFFF).
 */
uint32_t
gj_crc32c_final_u(uint32_t u32State)
{
	(void)NULL;
	return b3652_final(u32State);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_crc32c_final_u(uint32_t u32State)
    __attribute__((alias("gj_crc32c_final_u")));
