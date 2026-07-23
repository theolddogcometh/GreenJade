/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4564: map base64 char to sextet value (_u).
 *
 * Surface (unique symbols):
 *   unsigned gj_b64_char_val_u(unsigned c);
 *     - RFC 4648 §4 alphabet → 0..63; invalid (incl. pad '=') → 0xff.
 *   unsigned __gj_b64_char_val_u  (alias)
 *   __libcgj_batch4564_marker = "libcgj-batch4564"
 *
 * Exclusive continuum CREATE-ONLY (4561-4570). Distinct from private
 * b258_b64_val (batch258) — public _u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4564_marker[] = "libcgj-batch4564";

/* ---- freestanding helpers ---------------------------------------------- */

static unsigned
b4564_char_val(unsigned uC)
{
	if (uC >= (unsigned)'A' && uC <= (unsigned)'Z') {
		return uC - (unsigned)'A';
	}
	if (uC >= (unsigned)'a' && uC <= (unsigned)'z') {
		return (uC - (unsigned)'a') + 26u;
	}
	if (uC >= (unsigned)'0' && uC <= (unsigned)'9') {
		return (uC - (unsigned)'0') + 52u;
	}
	if (uC == (unsigned)'+') {
		return 62u;
	}
	if (uC == (unsigned)'/') {
		return 63u;
	}
	return 0xffu;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_b64_char_val_u - decode one standard base64 alphabet character.
 *
 * c: code unit to map
 *
 * Returns sextet 0..63, or 0xff when c is not in the §4 alphabet.
 * Self-contained; no parent wires.
 */
unsigned
gj_b64_char_val_u(unsigned c)
{
	(void)NULL;
	return b4564_char_val(c);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_b64_char_val_u(unsigned c)
    __attribute__((alias("gj_b64_char_val_u")));
