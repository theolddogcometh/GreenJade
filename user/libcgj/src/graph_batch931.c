/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch931: uint8_t from boolean (0/1).
 *
 * Surface (unique symbols):
 *   uint8_t gj_u8_from_bool(int f);
 *     — Return (uint8_t)1 if f is non-zero, else 0.
 *   uint8_t __gj_u8_from_bool  (alias)
 *   __libcgj_batch931_marker = "libcgj-batch931"

 *
 * Does NOT define gj_bool_to_u8 (batch929) — inverse-named dual surface;
 * both may coexist at link time.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch931_marker[] = "libcgj-batch931";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u8_from_bool — convert a C truthiness value to uint8_t 0 or 1.
 *
 * f: any int treated as boolean
 *
 * Distinct symbol from gj_bool_to_u8; same mathematical mapping.
 */
uint8_t
gj_u8_from_bool(int fVal)
{
	return fVal ? (uint8_t)1u : (uint8_t)0u;
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_u8_from_bool(int fVal)
    __attribute__((alias("gj_u8_from_bool")));
