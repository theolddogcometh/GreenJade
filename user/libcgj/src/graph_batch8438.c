/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8438: CRC zero-length integer stub (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_crc_zero_len_u_8438(uint32_t init, uint32_t n);
 *     - Empty-message CRC path: if n == 0 return init (pre-xor
 *       residual for zero bytes); else return 0 (not a zero-length
 *       path). Soft integer helper.
 *   uint32_t __gj_crc_zero_len_u_8438  (alias)
 *   __libcgj_batch8438_marker = "libcgj-batch8438"
 *
 * Exclusive continuum CREATE-ONLY (8431-8440: crc/poly integer stubs).
 * Unique gj_crc_zero_len_u_8438 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8438_marker[] = "libcgj-batch8438";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8438_zero_len(uint32_t u32Init, uint32_t u32N)
{
	if (u32N == 0u) {
		return u32Init;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc_zero_len_u_8438 - empty-message residual probe.
 *
 * init: CRC init / residual before first byte
 * n: message length in bytes
 *
 * n == 0 → init; else → 0. Soft integer helper. No parent wires.
 */
uint32_t
gj_crc_zero_len_u_8438(uint32_t u32Init, uint32_t u32N)
{
	(void)NULL;
	return b8438_zero_len(u32Init, u32N);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_crc_zero_len_u_8438(uint32_t u32Init, uint32_t u32N)
    __attribute__((alias("gj_crc_zero_len_u_8438")));
