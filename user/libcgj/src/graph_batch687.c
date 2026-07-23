/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch687: pure binary string → uint64_t.
 *
 * Surface (unique symbols):
 *   int gj_parse_bin_u64(const char *s, uint64_t *out);
 *     — Parse a full-string ASCII binary integer into *out.
 *       On success write value and return 0.
 *       On failure return -1 (no write). No errno (freestanding).
 *   int __gj_parse_bin_u64  (alias)
 *   __libcgj_batch687_marker = "libcgj-batch687"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch687_marker[] = "libcgj-batch687";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_parse_bin_u64 — full-string binary ASCII → uint64_t.
 *
 * Grammar (full-string match, no leading/trailing junk):
 *   [01]+
 *
 * Edge cases:
 *   s == NULL || out == NULL → -1
 *   empty / no bits / non-binary junk / overflow past UINT64_MAX → -1
 *   "0", "00", "000" → 0
 *   leading '+' / '-' / whitespace / "0b" prefix not accepted
 *   more than 64 significant bits after leading zeros → -1
 *
 * Examples:
 *   "0"                                  → 0
 *   "1"                                  → 1
 *   "1010"                               → 10
 *   "1111111111111111111111111111111111111111111111111111111111111111"
 *                                        → UINT64_MAX (64 ones)
 *   "10000000000000000000000000000000000000000000000000000000000000000"
 *                                        → -1 (65 bits, overflow)
 *   "" / "2" / "1x" / " 1" / "0b1"       → -1
 */
int
gj_parse_bin_u64(const char *s, uint64_t *out)
{
	const char *p;
	uint64_t u;
	int nDig;

	if (s == NULL || out == NULL) {
		return -1;
	}

	p = s;
	if (*p != '0' && *p != '1') {
		return -1;
	}

	u = 0ull;
	nDig = 0;
	while (*p == '0' || *p == '1') {
		uint64_t uBit = (uint64_t)(*p - '0');

		/* overflow if u would exceed UINT64_MAX after <<1 | bit */
		if (u > (UINT64_MAX >> 1)) {
			return -1;
		}
		u = (u << 1) | uBit;
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

int __gj_parse_bin_u64(const char *s, uint64_t *out)
    __attribute__((alias("gj_parse_bin_u64")));
