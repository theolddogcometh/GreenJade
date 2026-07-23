/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8457: ASCII hex digit predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hex_is_digit_u_8457(int c);
 *     - Return 1 if c is a hex digit ('0'-'9', 'a'-'f', 'A'-'F'),
 *       else 0.
 *   uint32_t __gj_hex_is_digit_u_8457  (alias)
 *   __libcgj_batch8457_marker = "libcgj-batch8457"
 *
 * Exclusive continuum CREATE-ONLY (8451-8460: hex encode/decode stubs).
 * Unique gj_hex_is_digit_u_8457 surface only; no multi-def. Predicate
 * companion to gj_hex_nibble_u_8452 (which returns the nibble value).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8457_marker[] = "libcgj-batch8457";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if c is 0-9 / a-f / A-F, else 0. */
static uint32_t
b8457_is_digit(int nC)
{
	if (nC >= '0' && nC <= '9') {
		return 1u;
	}
	if (nC >= 'a' && nC <= 'f') {
		return 1u;
	}
	if (nC >= 'A' && nC <= 'F') {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hex_is_digit_u_8457 - predicate: is c an ASCII hex digit?
 *
 * c: candidate character (typically an unsigned char promoted to int).
 *
 * Returns 1 for 0-9 / a-f / A-F, else 0. Pure; no side effects.
 * No parent wires.
 */
uint32_t
gj_hex_is_digit_u_8457(int nC)
{
	(void)NULL;
	return b8457_is_digit(nC);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hex_is_digit_u_8457(int nC)
    __attribute__((alias("gj_hex_is_digit_u_8457")));
