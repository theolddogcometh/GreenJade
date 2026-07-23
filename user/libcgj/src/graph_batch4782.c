/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4782: config falsy token predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_cfg_is_false_u(const char *s);
 *     - Full-string case-insensitive ASCII match against:
 *         no / false / 0 / off  → 1
 *       Any other token (including NULL / empty) → 0.
 *   int __gj_cfg_is_false_u  (alias)
 *   __libcgj_batch4782_marker = "libcgj-batch4782"
 *
 * Exclusive continuum CREATE-ONLY (4781-4790: cfg_is_true_u,
 * cfg_is_false_u, cfg_parse_u32_u, cfg_parse_u64_u, cfg_skip_ws_u,
 * cfg_is_key_char_u, cfg_is_sep_u, cfg_hash_key_u,
 * cfg_line_is_comment_u, batch_id_4790). Unique gj_cfg_is_false_u
 * surface only; no multi-def. Distinct from gj_parse_bool family —
 * predicate form (0/1, includes "off"), no -1 third state. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4782_marker[] = "libcgj-batch4782";

/* ---- freestanding helpers ---------------------------------------------- */

static char
b4782_tolower_ascii(char ch)
{
	if (ch >= 'A' && ch <= 'Z') {
		return (char)(ch - 'A' + 'a');
	}
	return ch;
}

/*
 * b4782_eq_ci — case-insensitive full-string ASCII compare to literal.
 * Returns 1 on match, 0 otherwise.
 */
static int
b4782_eq_ci(const char *sz, const char *szLit)
{
	size_t i;

	if (sz == NULL || szLit == NULL) {
		return 0;
	}
	i = 0u;
	for (;;) {
		char chA = b4782_tolower_ascii(sz[i]);
		char chB = b4782_tolower_ascii(szLit[i]);

		if (chA != chB) {
			return 0;
		}
		if (chA == '\0') {
			return 1;
		}
		i++;
	}
}

static int
b4782_is_false(const char *sz)
{
	if (sz == NULL) {
		return 0;
	}
	if (b4782_eq_ci(sz, "no") || b4782_eq_ci(sz, "false") ||
	    b4782_eq_ci(sz, "0") || b4782_eq_ci(sz, "off")) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cfg_is_false_u - 1 if s is a config falsy token, else 0.
 *
 * Accepted (case-insensitive ASCII, full-string): "no", "false", "0", "off".
 *
 * Edge cases:
 *   s == NULL / "" / "true" / "1" / "on" / "no " → 0
 *   "NO" / "False" / "OFF" → 1
 *
 * No parent wires.
 */
int
gj_cfg_is_false_u(const char *sz)
{
	(void)NULL;
	return b4782_is_false(sz);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_cfg_is_false_u(const char *sz)
    __attribute__((alias("gj_cfg_is_false_u")));
