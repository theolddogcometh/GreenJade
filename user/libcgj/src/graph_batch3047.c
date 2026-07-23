/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3047: environment variable value predicate (uint).
 *
 * Surface (unique symbols):
 *   uint32_t gj_env_val_ok_u(const char *val);
 *     - Return 1 if val is a soft-safe environment value: non-NULL,
 *       length within 4096 octets, and contains no C0 control octets
 *       other than horizontal tab (0x09). Empty string is allowed.
 *       Unsigned form.
 *   uint32_t __gj_env_val_ok_u  (alias)
 *   __libcgj_batch3047_marker = "libcgj-batch3047"
 *
 * Milestone 3050 exclusive CREATE-ONLY (3041-3050). Unique
 * gj_env_val_ok_u surface only; no multi-def. Distinct from
 * gj_env_key_ok_u (batch3046 name form), gj_env_parse_line (batch82),
 * and gj_shell_token_ok_u (batch3048). No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3047_marker[] = "libcgj-batch3047";

/* Inclusive scan budget for env value octets. */
#define B3047_SCAN_MAX  4096u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if u8C is a disallowed control (C0 except HT). DEL also rejected. */
static int
b3047_is_bad_ctrl(unsigned char u8C)
{
	if (u8C == (unsigned char)'\t') {
		return 0;
	}
	if (u8C < 0x20u) {
		return 1;
	}
	if (u8C == 0x7Fu) {
		return 1;
	}
	return 0;
}

/* 1u if szVal is soft-safe within scan budget. */
static uint32_t
b3047_val_ok(const char *szVal)
{
	size_t i;

	if (szVal == NULL) {
		return 0u;
	}
	i = 0u;
	while (i < B3047_SCAN_MAX && szVal[i] != '\0') {
		if (b3047_is_bad_ctrl((unsigned char)szVal[i])) {
			return 0u;
		}
		i++;
	}
	if (i >= B3047_SCAN_MAX && szVal[i] != '\0') {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_env_val_ok_u - 1 if val is a soft-safe env value string.
 *
 * val: NUL-terminated candidate value (may be NULL)
 *
 * Empty is ok. Rejects NULL, over-budget, C0 controls (except HT), DEL.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_env_val_ok_u(const char *szVal)
{
	(void)NULL;
	return b3047_val_ok(szVal);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_env_val_ok_u(const char *szVal)
    __attribute__((alias("gj_env_val_ok_u")));
