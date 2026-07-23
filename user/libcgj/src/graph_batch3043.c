/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3043: ASCII case-folded octet equality (uint).
 *
 * Surface (unique symbols):
 *   uint32_t gj_ascii_fold_eq_uc(unsigned char a, unsigned char b);
 *     - Return 1 if a and b are equal after ASCII case fold (A-Z ->
 *       a-z), else 0. Pure scalar; no string walk, no locale. Unsigned
 *       form. Renamed from gj_ascii_fold_eq_u (collision with batch2419).
 *   uint32_t __gj_ascii_fold_eq_uc  (alias)
 *   __libcgj_batch3043_marker = "libcgj-batch3043"
 *
 * Milestone 3050 exclusive CREATE-ONLY (3041-3050). Unique
 * gj_ascii_fold_eq_uc surface only; no multi-def. Distinct from
 * gj_ascii_fold_eq_u (batch2419 int form), gj_ascii_fold_eq
 * (batch1024 strings), and gj_ascii_fold_eq_n (batch1629). No parent
 * wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3043_marker[] = "libcgj-batch3043";

/* ---- freestanding helpers ---------------------------------------------- */

/* ASCII A-Z -> a-z; all other octets unchanged. */
static unsigned char
b3043_fold(unsigned char u8Ch)
{
	if (u8Ch >= (unsigned char)'A' && u8Ch <= (unsigned char)'Z') {
		return (unsigned char)(u8Ch + (unsigned char)('a' - 'A'));
	}
	return u8Ch;
}

/* 1u if folded octets match. */
static uint32_t
b3043_eq(unsigned char u8A, unsigned char u8B)
{
	if (b3043_fold(u8A) == b3043_fold(u8B)) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ascii_fold_eq_uc - 1 if two octets match under ASCII case fold.
 *
 * a, b: input octets
 *
 * Folds A-Z to a-z on both sides, then compares for equality.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_ascii_fold_eq_uc(unsigned char u8A, unsigned char u8B)
{
	(void)NULL;
	return b3043_eq(u8A, u8B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ascii_fold_eq_uc(unsigned char u8A, unsigned char u8B)
    __attribute__((alias("gj_ascii_fold_eq_uc")));
