/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4785: config leading-whitespace skip offset (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_cfg_skip_ws_u(const char *s);
 *     - Return the byte offset from s to the first non-horizontal-ws
 *       character (or to the terminating NUL). Horizontal ws is
 *       space / tab / form-feed / vertical-tab. NULL → 0.
 *   size_t __gj_cfg_skip_ws_u  (alias)
 *   __libcgj_batch4785_marker = "libcgj-batch4785"
 *
 * Exclusive continuum CREATE-ONLY (4781-4790: cfg_is_true_u,
 * cfg_is_false_u, cfg_parse_u32_u, cfg_parse_u64_u, cfg_skip_ws_u,
 * cfg_is_key_char_u, cfg_is_sep_u, cfg_hash_key_u,
 * cfg_line_is_comment_u, batch_id_4790). Unique gj_cfg_skip_ws_u
 * surface only; no multi-def. Offset return (not a pointer) so callers
 * can index without parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4785_marker[] = "libcgj-batch4785";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b4785_is_hws(unsigned char ch)
{
	return ch == ' ' || ch == '\t' || ch == '\f' || ch == '\v';
}

/*
 * b4785_skip_ws — count of leading horizontal whitespace bytes.
 */
static size_t
b4785_skip_ws(const char *sz)
{
	size_t iOff;

	if (sz == NULL) {
		return (size_t)0u;
	}
	iOff = 0u;
	while (sz[iOff] != '\0' &&
	       b4785_is_hws((unsigned char)sz[iOff])) {
		iOff++;
	}
	return iOff;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cfg_skip_ws_u - leading horizontal-ws byte offset into s.
 *
 * s: NUL-terminated config text (may be NULL)
 *
 * Returns the index of the first non-horizontal-ws char, or the index
 * of the terminating NUL when the whole string is horizontal ws.
 * Horizontal whitespace: ' ', '\\t', '\\f', '\\v' (not '\\n' / '\\r').
 *
 * Edge cases:
 *   s == NULL → 0
 *   "" / "x"  → 0
 *   "  key"   → 2
 *   "\\t\\t"  → 2 (offset of NUL)
 *
 * No parent wires.
 */
size_t
gj_cfg_skip_ws_u(const char *sz)
{
	(void)NULL;
	return b4785_skip_ws(sz);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_cfg_skip_ws_u(const char *sz)
    __attribute__((alias("gj_cfg_skip_ws_u")));
