/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2485: freestanding on/off boolean token parse.
 *
 * Surface (unique symbols):
 *   int gj_parse_bool_onoff(const char *s);
 *     — Full-string case-insensitive ASCII match against:
 *         on  → 1
 *         off → 0
 *       Any other token (including NULL / empty) → -1.
 *   int __gj_parse_bool_onoff  (alias)
 *   __libcgj_batch2485_marker = "libcgj-batch2485"
 *
 * Distinct from gj_parse_bool (batch448), gj_parse_bool_yn (batch2484),
 * and gj_parse_bool_strict (batch2141). Post-2480 parse exclusive wave
 * (2481-2490).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2485_marker[] = "libcgj-batch2485";

/* ---- freestanding helpers ---------------------------------------------- */

static char
b2485_tolower_ascii(char ch)
{
	if (ch >= 'A' && ch <= 'Z') {
		return (char)(ch - 'A' + 'a');
	}
	return ch;
}

/*
 * b2485_eq_ci — case-insensitive full-string ASCII compare to literal.
 * Returns 1 on match, 0 otherwise.
 */
static int
b2485_eq_ci(const char *sz, const char *szLit)
{
	size_t i;

	if (sz == NULL || szLit == NULL) {
		return 0;
	}
	i = 0u;
	for (;;) {
		char chA = b2485_tolower_ascii(sz[i]);
		char chB = b2485_tolower_ascii(szLit[i]);

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
 * gj_parse_bool_onoff — on/off token → 1 / 0 / -1.
 *
 * Accepted truthy (case-insensitive ASCII): "on"
 * Accepted falsy  (case-insensitive ASCII): "off"
 *
 * Edge cases:
 *   s == NULL → -1
 *   "" / "true" / "1" / "yes" / "on " → -1
 *   "ON" / "Off" / "OFF" → accepted
 */
int
gj_parse_bool_onoff(const char *sz)
{
	(void)NULL;

	if (sz == NULL) {
		return -1;
	}

	if (b2485_eq_ci(sz, "on")) {
		return 1;
	}
	if (b2485_eq_ci(sz, "off")) {
		return 0;
	}
	return -1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_parse_bool_onoff(const char *sz)
    __attribute__((alias("gj_parse_bool_onoff")));
