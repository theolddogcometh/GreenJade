/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8449: base64 stream character ok predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_b64_char_ok_u_8449(unsigned c);
 *     - 1 if c is in A-Z / a-z / 0-9 / '+' / '/' / '=', else 0.
 *       Accepts pad for stream-validation use (distinct from
 *       alphabet-only gj_b64_is_char_u).
 *   int __gj_b64_char_ok_u_8449  (alias)
 *   __libcgj_batch8449_marker = "libcgj-batch8449"
 *
 * Base64 alphabet stubs wave (8441-8450). Distinct from
 * gj_b64_is_char_u / gj_b64_is_pad_u / gj_base64url_char_ok — unique
 * _8449 surface; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8449_marker[] = "libcgj-batch8449";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b8449_char_ok(unsigned uC)
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
	if (uC == (unsigned)'+' || uC == (unsigned)'/' ||
	    uC == (unsigned)'=') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_b64_char_ok_u_8449 - 1 if c is a base64 alphabet or pad character.
 *
 * c: code unit to classify (typically a char promoted to unsigned)
 *
 * Returns 1 for A-Za-z0-9+/=, else 0. Self-contained; no parent wires.
 */
int
gj_b64_char_ok_u_8449(unsigned c)
{
	(void)NULL;
	return b8449_char_ok(c);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_b64_char_ok_u_8449(unsigned c)
    __attribute__((alias("gj_b64_char_ok_u_8449")));
