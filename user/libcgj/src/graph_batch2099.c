/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2099: freestanding ULID Crockford char probe.
 *
 * Surface (unique symbols):
 *   int gj_ulid_crockford_char_ok(int c);
 *     - Return 1 if c is a Crockford Base32 character used by ULIDs:
 *       0-9, A-H, J-K, M-N, P-T, V-Z (case-insensitive for letters;
 *       I/L/O/U rejected). Else 0.
 *   int __gj_ulid_crockford_char_ok  (alias)
 *   __libcgj_batch2099_marker = "libcgj-batch2099"
 *
 * Post-2000 id exclusive wave (2091-2100). Theme surface gj_ulid_crockford_char_ok
 * matches the Crockford alphabet predicate. Note: an earlier copy exists
 * in batch1179 under the same public name - keep only one in a link set.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2099_marker[] = "libcgj-batch2099";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2099_char_ok(int nC)
{
	int nCh;

	if (nC >= '0' && nC <= '9') {
		return 1;
	}
	if (nC >= 'a' && nC <= 'z') {
		nCh = nC - ('a' - 'A');
	} else {
		nCh = nC;
	}
	if (nCh < 'A' || nCh > 'Z') {
		return 0;
	}
	if (nCh == 'I' || nCh == 'L' || nCh == 'O' || nCh == 'U') {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ulid_crockford_char_ok - 1 iff c is Crockford Base32 (ULID alphabet).
 *
 * Accepted: 0123456789ABCDEFGHJKMNPQRSTVWXYZ and lowercase letters.
 * Rejected: I L O U / i l o u and all other characters.
 *
 * Does not call libc.
 */
int
gj_ulid_crockford_char_ok(int nC)
{
	(void)NULL;
	return b2099_char_ok(nC);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ulid_crockford_char_ok(int nC)
    __attribute__((alias("gj_ulid_crockford_char_ok")));
