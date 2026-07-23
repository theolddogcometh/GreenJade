/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2652: gj_crc32_final_u - IEEE CRC-32 stream final.
 *
 * Surface (unique symbols):
 *   uint32_t gj_crc32_final_u(uint32_t state);
 *     - Finalize a running IEEE CRC-32 internal state by XOR with
 *       0xFFFFFFFF (xorout). Empty stream with init state 0xFFFFFFFF
 *       yields 0x00000000 (zlib/IEEE one-shot empty).
 *   uint32_t __gj_crc32_final_u  (alias)
 *   __libcgj_batch2652_marker = "libcgj-batch2652"
 *
 * Companion to gj_crc32_update_u (batch2651). Streaming checksum wave
 * (2651-2660). Does NOT redefine crc32 / crc32_z / gj_crc32_update_u.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2652_marker[] = "libcgj-batch2652";

/* IEEE / zlib CRC-32 final XOR-out. */
#define B2652_XOROUT 0xFFFFFFFFu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2652_final(uint32_t u32State)
{
	return u32State ^ B2652_XOROUT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc32_final_u - finalize a streaming IEEE CRC-32.
 *
 * state: internal CRC from gj_crc32_update_u (or init 0xFFFFFFFF if empty)
 *
 * Returns the finalized 32-bit CRC (state XOR 0xFFFFFFFF).
 */
uint32_t
gj_crc32_final_u(uint32_t u32State)
{
	(void)NULL;
	return b2652_final(u32State);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_crc32_final_u(uint32_t u32State)
    __attribute__((alias("gj_crc32_final_u")));
