/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4322: freestanding C-string equality
 * (wave-unique rename).
 *
 * Surface (unique symbols):
 *   int gj_str_eq_u2(const char *a, const char *b);
 *     - Return 1 if a and b are equal (same sequence of octets up to
 *       and including the first NUL), else 0. Both NULL → 1; exactly
 *       one NULL → 0.
 *       Renamed from gj_str_eq_u (collision with batch2622).
 *   int __gj_str_eq_u2  (alias)
 *   __libcgj_batch4322_marker = "libcgj-batch4322"
 *
 * Exclusive continuum CREATE-ONLY (4321-4330 string utils). Distinct
 * from gj_str_eq (batch413) and gj_str_eq_u (batch2622) — unique
 * gj_str_eq_u2 surface only; no multi-def. Boolean equality only (not
 * ordering). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4322_marker[] = "libcgj-batch4322";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b4322_eq(const char *szA, const char *szB)
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
 * gj_str_eq_u2 - 1 if a equals b as C strings, else 0.
 *
 * Edge cases:
 *   a == NULL && b == NULL → 1
 *   exactly one of a, b is NULL → 0
 *   empty strings equal
 *
 * Does not call libc strcmp. Wave-unique rename: gj_str_eq_u is owned
 * by batch2622. Self-contained; no parent wires.
 */
int
gj_str_eq_u2(const char *a, const char *b)
{
	(void)NULL;
	return b4322_eq(a, b);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_eq_u2(const char *a, const char *b)
    __attribute__((alias("gj_str_eq_u2")));
