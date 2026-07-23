/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4789: config full-line comment predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_cfg_line_is_comment_u(const char *s);
 *     - 1 if the first non-horizontal-ws character of s is '#' or ';',
 *       else 0. NULL / empty / pure-ws lines → 0.
 *   int __gj_cfg_line_is_comment_u  (alias)
 *   __libcgj_batch4789_marker = "libcgj-batch4789"
 *
 * Exclusive continuum CREATE-ONLY (4781-4790: cfg_is_true_u,
 * cfg_is_false_u, cfg_parse_u32_u, cfg_parse_u64_u, cfg_skip_ws_u,
 * cfg_is_key_char_u, cfg_is_sep_u, cfg_hash_key_u,
 * cfg_line_is_comment_u, batch_id_4790). Unique gj_cfg_line_is_comment_u
 * surface only; no multi-def. Distinct from gj_toml_is_comment_u /
 * gj_ini_is_comment_char — full-line form with leading-ws skip, both
 * '#' and ';'. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4789_marker[] = "libcgj-batch4789";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b4789_is_hws(unsigned char ch)
{
	return ch == ' ' || ch == '\t' || ch == '\f' || ch == '\v';
}

/*
 * b4789_line_is_comment — 1 if first non-hws char is '#' or ';'.
 */
static int
b4789_line_is_comment(const char *sz)
{
	size_t i;

	if (sz == NULL) {
		return 0;
	}
	i = 0u;
	while (sz[i] != '\0' && b4789_is_hws((unsigned char)sz[i])) {
		i++;
	}
	if (sz[i] == '#' || sz[i] == ';') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cfg_line_is_comment_u - 1 if s is a full-line config comment.
 *
 * A line is a comment when, after skipping leading horizontal whitespace
 * (' ', '\\t', '\\f', '\\v'), the next character is '#' or ';'.
 *
 * Edge cases:
 *   s == NULL / "" / "   " / "key=1" → 0
 *   "# comment" / "  ; note" / ";x"  → 1
 *   "x# not full-line"               → 0
 *
 * No parent wires.
 */
int
gj_cfg_line_is_comment_u(const char *sz)
{
	(void)NULL;
	return b4789_line_is_comment(sz);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_cfg_line_is_comment_u(const char *sz)
    __attribute__((alias("gj_cfg_line_is_comment_u")));
