/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4323: freestanding C-string inequality
 * predicate (wave-unique rename).
 *
 * Surface (unique symbols):
 *   int gj_str_neq_u2(const char *a, const char *b);
 *     - Return 1 if a and b are not equal as C strings, else 0.
 *       Both NULL → 0 (equal, so not-neq). Exactly one NULL → 1.
 *       Renamed from gj_str_neq_u (collision with batch2754).
 *   int __gj_str_neq_u2  (alias)
 *   __libcgj_batch4323_marker = "libcgj-batch4323"
 *
 * Exclusive continuum CREATE-ONLY (4321-4330 string utils). Companion
 * inverse of gj_str_eq_u2 (batch4322). Distinct from gj_str_neq_u
 * (batch2754) — unique gj_str_neq_u2 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4323_marker[] = "libcgj-batch4323";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b4323_eq(const char *szA, const char *szB)
{
	size_t i;

	if (szA == NULL && szB == NULL) {
		return 1;
	}
	if (szA == NULL || szB == NULL) {
		return 0;
	}

	i = 0u;
	for (;;) {
		if ((unsigned char)szA[i] != (unsigned char)szB[i]) {
			return 0;
		}
		if (szA[i] == '\0') {
			return 1;
		}
		i++;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_neq_u2 - 1 if a and b differ as C strings, else 0.
 *
 * Edge cases:
 *   a == NULL && b == NULL → 0 (equal)
 *   exactly one of a, b is NULL → 1
 *   empty strings equal → 0
 *
 * Comparison is unsigned-byte exact. Does not call libc strcmp.
 * Wave-unique rename: gj_str_neq_u is owned by batch2754.
 * Self-contained; no parent wires.
 */
int
gj_str_neq_u2(const char *a, const char *b)
{
	(void)NULL;
	if (b4323_eq(a, b) != 0) {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_neq_u2(const char *a, const char *b)
    __attribute__((alias("gj_str_neq_u2")));
