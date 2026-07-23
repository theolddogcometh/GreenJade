/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4786: config key character predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_cfg_is_key_char_u(unsigned c);
 *     - 1 if c is a legal config key character:
 *         A-Z a-z 0-9 _ - . /
 *       else 0. Intended for bare key=value / key:value scanners.
 *   int __gj_cfg_is_key_char_u  (alias)
 *   __libcgj_batch4786_marker = "libcgj-batch4786"
 *
 * Exclusive continuum CREATE-ONLY (4781-4790: cfg_is_true_u,
 * cfg_is_false_u, cfg_parse_u32_u, cfg_parse_u64_u, cfg_skip_ws_u,
 * cfg_is_key_char_u, cfg_is_sep_u, cfg_hash_key_u,
 * cfg_line_is_comment_u, batch_id_4790). Unique gj_cfg_is_key_char_u
 * surface only; no multi-def. Separators (= / :) and whitespace are
 * not key chars. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4786_marker[] = "libcgj-batch4786";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b4786_is_key_char(unsigned uC)
{
	if (uC >= (unsigned)'A' && uC <= (unsigned)'Z') {
		return 1;
	}
	if (uC >= (unsigned)'a' && uC <= (unsigned)'z') {
		return 1;
	}
	if (uC >= (unsigned)'0' && uC <= (unsigned)'9') {
		return 1;
	}
	if (uC == (unsigned)'_' || uC == (unsigned)'-' ||
	    uC == (unsigned)'.' || uC == (unsigned)'/') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cfg_is_key_char_u - 1 if c is a legal config key character.
 *
 * Legal set: [A-Za-z0-9_./-]
 *
 * Not key chars: '=', ':', whitespace, '#', ';', NUL, control bytes,
 * and any other punctuation.
 *
 * Edge cases:
 *   c == 0 / ' ' / '=' / ':' / '#' → 0
 *   c == 'a' / 'Z' / '0' / '_' / '-' / '.' / '/' → 1
 *
 * No parent wires.
 */
int
gj_cfg_is_key_char_u(unsigned uC)
{
	(void)NULL;
	return b4786_is_key_char(uC);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_cfg_is_key_char_u(unsigned uC)
    __attribute__((alias("gj_cfg_is_key_char_u")));
