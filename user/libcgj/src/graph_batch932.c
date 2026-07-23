/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch932: pack eight bools into one byte.
 *
 * Surface (unique symbols):
 *   uint8_t gj_pack_bools8(int f0, int f1, int f2, int f3,
 *                          int f4, int f5, int f6, int f7);
 *     — Bit i of the result is 1 iff fi is non-zero (i = 0..7, LSB = f0).
 *   uint8_t __gj_pack_bools8  (alias)
 *   __libcgj_batch932_marker = "libcgj-batch932"
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch932_marker[] = "libcgj-batch932";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pack_bools8 — pack eight independent truthiness values into a byte.
 *
 * f0..f7: each non-zero contributes bit (1 << index); f0 is the LSB.
 * Example: (1,0,1,0,0,0,0,0) → 0x05.
 */
uint8_t
gj_pack_bools8(int f0, int f1, int f2, int f3, int f4, int f5, int f6, int f7)
{
	uint8_t u8Out;

	u8Out = 0u;
	if (f0 != 0) {
		u8Out = (uint8_t)(u8Out | (uint8_t)0x01u);
	}
	if (f1 != 0) {
		u8Out = (uint8_t)(u8Out | (uint8_t)0x02u);
	}
	if (f2 != 0) {
		u8Out = (uint8_t)(u8Out | (uint8_t)0x04u);
	}
	if (f3 != 0) {
		u8Out = (uint8_t)(u8Out | (uint8_t)0x08u);
	}
	if (f4 != 0) {
		u8Out = (uint8_t)(u8Out | (uint8_t)0x10u);
	}
	if (f5 != 0) {
		u8Out = (uint8_t)(u8Out | (uint8_t)0x20u);
	}
	if (f6 != 0) {
		u8Out = (uint8_t)(u8Out | (uint8_t)0x40u);
	}
	if (f7 != 0) {
		u8Out = (uint8_t)(u8Out | (uint8_t)0x80u);
	}
	return u8Out;
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_pack_bools8(int f0, int f1, int f2, int f3, int f4, int f5,
    int f6, int f7)
    __attribute__((alias("gj_pack_bools8")));
