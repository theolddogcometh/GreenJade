/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3048: shell token safety predicate (uint).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_token_ok_u(const char *tok);
 *     - Return 1 if tok is a non-empty shell-safe token: no IFS,
 *       operators, quotes, or expansion metacharacters. Cap scan at
 *       4096 bytes. NULL / empty → 0. Unsigned form.
 *   uint32_t __gj_shell_token_ok_u  (alias)
 *   __libcgj_batch3048_marker = "libcgj-batch3048"
 *
 * Milestone 3050 exclusive CREATE-ONLY (3041-3050). Unique
 * gj_shell_token_ok_u surface only; no multi-def. Distinct from
 * gj_shell_needs_quote (batch1233 inverted empty policy),
 * gj_shell_is_metachar (batch917 single char), and gj_env_key_ok_u
 * (batch3046). No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3048_marker[] = "libcgj-batch3048";

/* Inclusive scan budget for shell token octets. */
#define B3048_SCAN_MAX  4096u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if nCh is a shell metacharacter / IFS / expansion trigger. */
static int
b3048_is_meta(int nCh)
{
	switch (nCh) {
	case ' ':
	case '\t':
	case '\n':
	case '\r':
	case '|':
	case '&':
	case ';':
	case '(':
	case ')':
	case '<':
	case '>':
	case '*':
	case '?':
	case '[':
	case ']':
	case '{':
	case '}':
	case '\'':
	case '"':
	case '`':
	case '$':
	case '\\':
	case '#':
	case '!':
	case '~':
		return 1;
	default:
		return 0;
	}
}

/* 1u if szTok is non-empty and free of shell metacharacters. */
static uint32_t
b3048_token_ok(const char *szTok)
{
	size_t i;

	if (szTok == NULL || szTok[0] == '\0') {
		return 0u;
	}
	i = 0u;
	while (i < B3048_SCAN_MAX && szTok[i] != '\0') {
		unsigned char u8C = (unsigned char)szTok[i];

		/* Reject C0 controls and DEL. */
		if (u8C < 0x20u || u8C == 0x7Fu) {
			return 0u;
		}
		if (b3048_is_meta((int)u8C)) {
			return 0u;
		}
		i++;
	}
	if (i >= B3048_SCAN_MAX && szTok[i] != '\0') {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_token_ok_u - 1 if tok is a non-empty shell-safe token.
 *
 * tok: NUL-terminated candidate token (may be NULL)
 *
 * Does not call libc. No parent wires.
 */
uint32_t
gj_shell_token_ok_u(const char *szTok)
{
	(void)NULL;
	return b3048_token_ok(szTok);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_token_ok_u(const char *szTok)
    __attribute__((alias("gj_shell_token_ok_u")));
