/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4783: config decimal string → uint32_t (_u).
 *
 * Surface (unique symbols):
 *   int gj_cfg_parse_u32_u(const char *s, uint32_t *out);
 *     - Full-string ASCII decimal integer into *out.
 *       On success write value and return 1.
 *       On failure return 0 (no write). No errno (freestanding).
 *   int __gj_cfg_parse_u32_u  (alias)
 *   __libcgj_batch4783_marker = "libcgj-batch4783"
 *
 * Exclusive continuum CREATE-ONLY (4781-4790: cfg_is_true_u,
 * cfg_is_false_u, cfg_parse_u32_u, cfg_parse_u64_u, cfg_skip_ws_u,
 * cfg_is_key_char_u, cfg_is_sep_u, cfg_hash_key_u,
 * cfg_line_is_comment_u, batch_id_4790). Unique gj_cfg_parse_u32_u
 * surface only; no multi-def. Distinct from gj_parse_u32 /
 * gj_parse_u32_dec_n / gj_parse_u32_dec_full — returns 1 on ok (not
 * 0 / digit count / -1). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4783_marker[] = "libcgj-batch4783";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b4783_is_digit(char ch)
{
	return (ch >= '0' && ch <= '9') ? 1 : 0;
}

/*
 * b4783_parse_u32 — full-string decimal → uint32_t.
 * Returns 1 on success (*pOut written), 0 on failure (no write).
 */
static int
b4783_parse_u32(const char *sz, uint32_t *pOut)
{
	const char *pCur;
	uint32_t uVal;
	int nDig;

	if (sz == NULL || pOut == NULL) {
		return 0;
	}

	pCur = sz;
	if (!b4783_is_digit(*pCur)) {
		return 0;
	}

	uVal = 0u;
	nDig = 0;
	while (b4783_is_digit(*pCur)) {
		uint32_t uDig = (uint32_t)(*pCur - '0');

		if (uVal > (UINT32_MAX - uDig) / 10u) {
			return 0;
		}
		uVal = uVal * 10u + uDig;
		pCur++;
		nDig++;
	}
	if (nDig == 0) {
		return 0;
	}
	if (*pCur != '\0') {
		return 0;
	}

	*pOut = uVal;
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cfg_parse_u32_u - full-string decimal ASCII → uint32_t.
 *
 * Grammar (full-string match, no leading/trailing junk):
 *   [0-9]+
 *
 * Returns 1 on success (*out written), 0 on failure (no write).
 *
 * Edge cases:
 *   s == NULL || out == NULL → 0
 *   empty / no digits / non-digit junk / overflow past UINT32_MAX → 0
 *   "0", "00", "000" → 0 value, return 1
 *   leading '+' / '-' / whitespace not accepted
 *
 * Examples:
 *   "0"          → *out=0, return 1
 *   "42"         → *out=42, return 1
 *   "4294967295" → *out=UINT32_MAX, return 1
 *   "4294967296" → return 0 (overflow)
 *   "" / "x" / "1x" / " 1" → 0
 *
 * No parent wires.
 */
int
gj_cfg_parse_u32_u(const char *sz, uint32_t *pOut)
{
	(void)NULL;
	return b4783_parse_u32(sz, pOut);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_cfg_parse_u32_u(const char *sz, uint32_t *pOut)
    __attribute__((alias("gj_cfg_parse_u32_u")));
