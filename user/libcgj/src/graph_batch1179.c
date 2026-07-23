/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1179: Crockford Base32 ULID character predicate.
 *
 * Surface (unique symbols):
 *   int gj_ulid_char_ok(int c);
 *     — Return 1 if c is a Crockford Base32 character used by ULIDs:
 *       0-9, A-H, J-K, M-N, P-T, V-Z (case-insensitive for letters;
 *       I/L/O/U rejected). Else 0.
 *   int __gj_ulid_char_ok  (alias)
 *   __libcgj_batch1179_marker = "libcgj-batch1179"
 *
 * Distinct from gj_ulid_set_time / gj_ulid_get_time (batch469) —
 * avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

const char __libcgj_batch1179_marker[] = "libcgj-batch1179";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ulid_char_ok — 1 iff c is Crockford Base32 (ULID alphabet).
 *
 * Accepted: 0123456789ABCDEFGHJKMNPQRSTVWXYZ (and lowercase).
 * Rejected: I L O U / i l o u and all other characters.
 */
int
gj_ulid_char_ok(int c)
{
	int ch;

	if (c >= '0' && c <= '9') {
		return 1;
	}
	/* Normalize A-Z / a-z */
	if (c >= 'a' && c <= 'z') {
		ch = c - ('a' - 'A');
	} else {
		ch = c;
	}
	if (ch < 'A' || ch > 'Z') {
		return 0;
	}
	/* Exclude I, L, O, U */
	if (ch == 'I' || ch == 'L' || ch == 'O' || ch == 'U') {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ulid_char_ok(int c)
    __attribute__((alias("gj_ulid_char_ok")));
