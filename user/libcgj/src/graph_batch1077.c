/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1077: freestanding decimal → uint32_t.
 *
 * Surface (unique symbols):
 *   int gj_parse_u32(const char *s, uint32_t *out);
 *     — Parse full-string ASCII decimal into *out. Success → 0;
 *       failure → -1 (no write). No leading sign.
 *   int __gj_parse_u32  (alias)
 *   __libcgj_batch1077_marker = "libcgj-batch1077"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1077_marker[] = "libcgj-batch1077";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_parse_u32 — full-string decimal ASCII → uint32_t.
 *
 * Grammar: [0-9]+
 * Overflow past UINT32_MAX → -1.
 */
int
gj_parse_u32(const char *sz, uint32_t *pOut)
{
	const char *p;
	uint32_t u;
	int nDig;

	if (sz == NULL || pOut == NULL) {
		return -1;
	}

	p = sz;
	if (*p < '0' || *p > '9') {
		return -1;
	}

	u = 0u;
	nDig = 0;
	while (*p >= '0' && *p <= '9') {
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

	*pOut = u;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_parse_u32(const char *sz, uint32_t *pOut)
    __attribute__((alias("gj_parse_u32")));
