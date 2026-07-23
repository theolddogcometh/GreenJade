/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2141: freestanding strict boolean token parse.
 *
 * Surface (unique symbols):
 *   int gj_parse_bool_strict(const char *s);
 *     - Full-string case-sensitive ASCII match against:
 *         "true"  -> 1
 *         "false" -> 0
 *       Any other token (including NULL, empty, yes/no/1/0, mixed case)
 *       -> -1. Distinct from gj_parse_bool (batch448, case-insensitive
 *       true/false/1/0/yes/no) and gj_parse_bool_ci (batch1078).
 *   int __gj_parse_bool_strict  (alias)
 *   __libcgj_batch2141_marker = "libcgj-batch2141"
 *
 * Post-2140 parse exclusive wave (2141-2150). Clean-room freestanding
 * pure C (integer/pointer only). Compiles with -ffreestanding -msse2
 * -Wall -Wextra -Werror. No malloc, no errno, no libc. No third-party
 * source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2141_marker[] = "libcgj-batch2141";

/* ---- helpers ----------------------------------------------------------- */

/*
 * b2141_eq - exact full-string ASCII compare to literal.
 * Returns 1 on match, 0 otherwise.
 */
static int
b2141_eq(const char *sz, const char *szLit)
{
	size_t i;

	if (sz == NULL || szLit == NULL) {
		return 0;
	}
	i = 0u;
	for (;;) {
		char chA = sz[i];
		char chB = szLit[i];

		if (chA != chB) {
			return 0;
		}
		if (chA == '\0') {
			return 1;
		}
		i++;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_parse_bool_strict - exact "true" / "false" token -> 1 / 0 / -1.
 *
 * Accepted truthy (case-sensitive): "true"
 * Accepted falsy  (case-sensitive): "false"
 *
 * Edge cases:
 *   s == NULL -> -1
 *   "" / "TRUE" / "True" / "1" / "yes" / "false " -> -1
 */
int
gj_parse_bool_strict(const char *sz)
{
	(void)NULL;

	if (sz == NULL) {
		return -1;
	}

	if (b2141_eq(sz, "true")) {
		return 1;
	}
	if (b2141_eq(sz, "false")) {
		return 0;
	}
	return -1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_parse_bool_strict(const char *sz)
    __attribute__((alias("gj_parse_bool_strict")));
