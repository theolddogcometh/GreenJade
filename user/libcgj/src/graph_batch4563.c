/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4563: test RFC 4648 §4 base64 alphabet char (_u).
 *
 * Surface (unique symbols):
 *   int gj_b64_is_char_u(unsigned c);
 *     - 1 if c is in A-Z / a-z / 0-9 / '+' / '/'; else 0. Does not
 *       accept pad '=' (see gj_b64_is_pad_u) or base64url -_ .
 *   int __gj_b64_is_char_u  (alias)
 *   __libcgj_batch4563_marker = "libcgj-batch4563"
 *
 * Exclusive continuum CREATE-ONLY (4561-4570). Distinct from decoder
 * internals in batch258 — predicate surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4563_marker[] = "libcgj-batch4563";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b4563_is_char(unsigned uC)
{
	if (uC >= (unsigned)'A' && uC <= (unsigned)'Z') {
		return 1;
	}
	if (uC >= (unsigned)'a' && uC <= (unsigned)'z') {
		return 1;
	}
	if (uC >= (unsigned)'0' && uC <= (unsigned)'9') {
		return 1;
	}
	if (uC == (unsigned)'+' || uC == (unsigned)'/') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_b64_is_char_u - 1 if c is a standard base64 alphabet character.
 *
 * c: code unit to classify (typically a char promoted to unsigned)
 *
 * Returns 1 for A-Za-z0-9+/, else 0. Self-contained; no parent wires.
 */
int
gj_b64_is_char_u(unsigned c)
{
	(void)NULL;
	return b4563_is_char(c);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_b64_is_char_u(unsigned c)
    __attribute__((alias("gj_b64_is_char_u")));
