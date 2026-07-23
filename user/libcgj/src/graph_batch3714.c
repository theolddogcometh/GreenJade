/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3714: full-string decimal string → uint32_t (_u).
 *
 * Surface (unique symbols):
 *   int gj_parse_u32_dec_u(const char *s, uint32_t *out);
 *     - Parse a full-string ASCII decimal integer into *out.
 *       On success write value and return 0.
 *       On failure return -1 (no write). No errno (freestanding).
 *   int __gj_parse_u32_dec_u  (alias)
 *   __libcgj_batch3714_marker = "libcgj-batch3714"
 *
 * Exclusive continuum CREATE-ONLY (3711-3720). Distinct from
 * gj_parse_u32 (batch1077), gj_parse_u32_dec_n (batch2142 bounded) —
 * unique _u surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3714_marker[] = "libcgj-batch3714";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b3714_is_digit(char ch)
{
	return (ch >= '0' && ch <= '9') ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_parse_u32_dec_u — full-string decimal ASCII → uint32_t.
 *
 * Grammar (full-string match, no leading/trailing junk):
 *   [0-9]+
 *
 * Edge cases:
 *   s == NULL || out == NULL → -1
 *   empty / no digits / non-digit junk / overflow past UINT32_MAX → -1
 *   "0", "00", "000" → 0
 *   leading '+' / '-' / whitespace not accepted
 *
 * Examples:
 *   "0"          → 0
 *   "42"         → 42
 *   "4294967295" → UINT32_MAX
 *   "4294967296" → -1 (overflow)
 *   "" / "x" / "1x" / " 1" → -1
 */
int
gj_parse_u32_dec_u(const char *s, uint32_t *out)
{
	const char *p;
	uint32_t u;
	int nDig;

	(void)NULL;

	if (s == NULL || out == NULL) {
		return -1;
	}

	p = s;
	if (!b3714_is_digit(*p)) {
		return -1;
	}

	u = 0u;
	nDig = 0;
	while (b3714_is_digit(*p)) {
		uint32_t uDig = (uint32_t)(*p - '0');

		if (u > (UINT32_MAX - uDig) / 10u) {
			return -1;
		}
		u = u * 10u + uDig;
		p++;
		nDig++;
	}
	if (nDig == 0) {
		return -1;
	}
	if (*p != '\0') {
		return -1;
	}

	*out = u;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_parse_u32_dec_u(const char *s, uint32_t *out)
    __attribute__((alias("gj_parse_u32_dec_u")));
