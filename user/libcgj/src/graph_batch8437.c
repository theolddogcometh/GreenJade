/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8437: CRC final-xor integer stub (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_crc_final_xor_u_8437(uint32_t crc, uint32_t xorout);
 *     - Apply catalog final XOR: return crc ^ xorout.
 *   uint32_t __gj_crc_final_xor_u_8437  (alias)
 *   __libcgj_batch8437_marker = "libcgj-batch8437"
 *
 * Exclusive continuum CREATE-ONLY (8431-8440: crc/poly integer stubs).
 * Unique gj_crc_final_xor_u_8437 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8437_marker[] = "libcgj-batch8437";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8437_final_xor(uint32_t u32Crc, uint32_t u32Xorout)
{
	return u32Crc ^ u32Xorout;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc_final_xor_u_8437 - apply CRC catalog final XOR.
 *
 * crc: residual before xorout
 * xorout: final XOR mask (e.g. 0xFFFFFFFF for CRC-32/ISO-HDLC)
 *
 * Returns crc ^ xorout. Soft integer helper. No parent wires.
 */
uint32_t
gj_crc_final_xor_u_8437(uint32_t u32Crc, uint32_t u32Xorout)
{
	(void)NULL;
	return b8437_final_xor(u32Crc, u32Xorout);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_crc_final_xor_u_8437(uint32_t u32Crc, uint32_t u32Xorout)
    __attribute__((alias("gj_crc_final_xor_u_8437")));
