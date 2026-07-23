/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1841: printable ASCII key predicate.
 *
 * Surface (unique symbols):
 *   int gj_key_is_printable_ascii(unsigned key);
 *     — Return 1 if key is a printable US-ASCII code point
 *       (0x20 SPACE .. 0x7E '~'), else 0. Pure data; no locale.
 *   int __gj_key_is_printable_ascii  (alias)
 *   __libcgj_batch1841_marker = "libcgj-batch1841"
 *
 * Keyboard/input exclusive pure-data wave (1841–1850).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1841_marker[] = "libcgj-batch1841";

/* Printable US-ASCII inclusive bounds (SPACE .. TILDE). */
#define B1841_PRINT_LO 0x20u
#define B1841_PRINT_HI 0x7eu

/* ---- freestanding helpers ---------------------------------------------- */

/* True when uKey is in the printable ASCII band. */
static int
b1841_is_printable(unsigned uKey)
{
	return (uKey >= B1841_PRINT_LO && uKey <= B1841_PRINT_HI) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_key_is_printable_ascii — 1 if key is printable US-ASCII.
 *
 * uKey: key value as unsigned code point / scancode-as-char
 * Returns 1 when 0x20 <= uKey <= 0x7E, else 0.
 */
int
gj_key_is_printable_ascii(unsigned uKey)
{
	return b1841_is_printable(uKey);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_key_is_printable_ascii(unsigned uKey)
    __attribute__((alias("gj_key_is_printable_ascii")));
