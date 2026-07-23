/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4787: config key/value separator predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_cfg_is_sep_u(unsigned c);
 *     - 1 if c is a key/value separator character: '=' or ':'.
 *       else 0.
 *   int __gj_cfg_is_sep_u  (alias)
 *   __libcgj_batch4787_marker = "libcgj-batch4787"
 *
 * Exclusive continuum CREATE-ONLY (4781-4790: cfg_is_true_u,
 * cfg_is_false_u, cfg_parse_u32_u, cfg_parse_u64_u, cfg_skip_ws_u,
 * cfg_is_key_char_u, cfg_is_sep_u, cfg_hash_key_u,
 * cfg_line_is_comment_u, batch_id_4790). Unique gj_cfg_is_sep_u
 * surface only; no multi-def. Accepts both INI-style '=' and
 * YAML/colon-style ':' separators. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4787_marker[] = "libcgj-batch4787";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b4787_is_sep(unsigned uC)
{
	if (uC == (unsigned)'=' || uC == (unsigned)':') {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cfg_is_sep_u - 1 if c is a key/value separator ('=' or ':').
 *
 * Accepted separators: '=' (INI/common), ':' (colon-style).
 *
 * Edge cases:
 *   c == '=' / ':' → 1
 *   c == 0 / ' ' / '#' / ';' / '-' → 0
 *
 * No parent wires.
 */
int
gj_cfg_is_sep_u(unsigned uC)
{
	(void)NULL;
	return b4787_is_sep(uC);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_cfg_is_sep_u(unsigned uC)
    __attribute__((alias("gj_cfg_is_sep_u")));
