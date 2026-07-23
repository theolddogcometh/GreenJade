/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4784: config decimal string → uint64_t (_u).
 *
 * Surface (unique symbols):
 *   int gj_cfg_parse_u64_u(const char *s, uint64_t *out);
 *     - Full-string ASCII decimal integer into *out.
 *       On success write value and return 1.
 *       On failure return 0 (no write). No errno (freestanding).
 *   int __gj_cfg_parse_u64_u  (alias)
 *   __libcgj_batch4784_marker = "libcgj-batch4784"
 *
 * Exclusive continuum CREATE-ONLY (4781-4790: cfg_is_true_u,
 * cfg_is_false_u, cfg_parse_u32_u, cfg_parse_u64_u, cfg_skip_ws_u,
 * cfg_is_key_char_u, cfg_is_sep_u, cfg_hash_key_u,
 * cfg_line_is_comment_u, batch_id_4790). Unique gj_cfg_parse_u64_u
 * surface only; no multi-def. Distinct from gj_parse_u64_dec /
 * gj_parse_u64_dec_n / gj_parse_u64_dec_full — returns 1 on ok (not
 * 0 / digit count / -1). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4784_marker[] = "libcgj-batch4784";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b4784_is_digit(char ch)
{
	return (ch >= '0' && ch <= '9') ? 1 : 0;
}

/*
 * b4784_parse_u64 — full-string decimal → uint64_t.
 * Returns 1 on success (*pOut written), 0 on failure (no write).
 */
static int
b4784_parse_u64(const char *sz, uint64_t *pOut)
{
	const char *pCur;
	uint64_t uVal;
	int nDig;

	if (sz == NULL || pOut == NULL) {
		return 0;
	}

	pCur = sz;
	if (!b4784_is_digit(*pCur)) {
		return 0;
	}

	uVal = 0ull;
	nDig = 0;
	while (b4784_is_digit(*pCur)) {
		uint64_t uDig = (uint64_t)(*pCur - '0');

		if (uVal > (UINT64_MAX - uDig) / 10ull) {
			return 0;
		}
		uVal = uVal * 10ull + uDig;
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
 * gj_cfg_parse_u64_u - full-string decimal ASCII → uint64_t.
 *
 * Grammar (full-string match, no leading/trailing junk):
 *   [0-9]+
 *
 * Returns 1 on success (*out written), 0 on failure (no write).
 *
 * Edge cases:
 *   s == NULL || out == NULL → 0
 *   empty / no digits / non-digit junk / overflow past UINT64_MAX → 0
 *   "0", "00", "000" → 0 value, return 1
 *   leading '+' / '-' / whitespace not accepted
 *
 * Examples:
 *   "0"                    → *out=0, return 1
 *   "42"                   → *out=42, return 1
 *   "18446744073709551615" → *out=UINT64_MAX, return 1
 *   "18446744073709551616" → return 0 (overflow)
 *   "" / "x" / "1x" / " 1" → 0
 *
 * No parent wires.
 */
int
gj_cfg_parse_u64_u(const char *sz, uint64_t *pOut)
{
	(void)NULL;
	return b4784_parse_u64(sz, pOut);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_cfg_parse_u64_u(const char *sz, uint64_t *pOut)
    __attribute__((alias("gj_cfg_parse_u64_u")));
