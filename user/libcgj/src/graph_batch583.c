/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch583: pure decimal string → uint64_t.
 *
 * Surface (unique symbols):
 *   int gj_parse_u64(const char *s, uint64_t *out);
 *     — Parse a full-string ASCII decimal integer into *out.
 *       On success write value and return 0.
 *       On failure return -1 (no write). No errno (freestanding).
 *   int __gj_parse_u64  (alias)
 *   __libcgj_batch583_marker = "libcgj-batch583"
 *
 * Avoid multi-def: gj_parse_u64_dec / __gj_parse_u64_dec live in
 * graph_batch390.c; gj_parse_u64_hex / __gj_parse_u64_hex live in
 * graph_batch392.c. This TU exports only the plain gj_parse_u64 surface.
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch583_marker[] = "libcgj-batch583";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_parse_u64 — full-string decimal ASCII → uint64_t.
 *
 * Grammar (full-string match, no leading/trailing junk):
 *   [0-9]+
 *
 * Edge cases:
 *   s == NULL || out == NULL → -1
 *   empty / no digits / non-digit junk / overflow past UINT64_MAX → -1
 *   "0", "00", "000" → 0
 *   leading '+' / '-' / whitespace not accepted
 *
 * Examples:
 *   "0"                  → 0
 *   "42"                 → 42
 *   "18446744073709551615" → UINT64_MAX
 *   "18446744073709551616" → -1 (overflow)
 *   "" / "x" / "1x" / " 1" → -1
 */
int
gj_parse_u64(const char *s, uint64_t *out)
{
	const char *p;
	uint64_t u;
	int nDig;

	if (s == NULL || out == NULL) {
		return -1;
	}

	p = s;
	if (*p < '0' || *p > '9') {
		return -1;
	}

	u = 0ull;
	nDig = 0;
	while (*p >= '0' && *p <= '9') {
		uint64_t uDig = (uint64_t)(*p - '0');

		if (u > (UINT64_MAX - uDig) / 10ull) {
			return -1;
		}
		u = u * 10ull + uDig;
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

int __gj_parse_u64(const char *s, uint64_t *out)
    __attribute__((alias("gj_parse_u64")));
