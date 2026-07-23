/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3046: environment variable key predicate (uint).
 *
 * Surface (unique symbols):
 *   uint32_t gj_env_key_ok_u(const char *key);
 *     - Return 1 if key is a non-empty POSIX-ish environment variable
 *       name: first char [A-Za-z_], subsequent [A-Za-z0-9_], else 0.
 *       NULL / empty → 0. Cap scan at 4096 bytes. Unsigned form.
 *   uint32_t __gj_env_key_ok_u  (alias)
 *   __libcgj_batch3046_marker = "libcgj-batch3046"
 *
 * Milestone 3050 exclusive CREATE-ONLY (3041-3050). Unique
 * gj_env_key_ok_u surface only; no multi-def. Distinct from
 * gj_env_is_name_char (batch916 per-octet body), gj_env_is_name_char_u
 * (batch1599), gj_str_is_ident (batch827), and gj_env_val_ok_u
 * (batch3047). No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3046_marker[] = "libcgj-batch3046";

/* Inclusive scan budget for env key octets. */
#define B3046_SCAN_MAX  4096u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b3046_is_start(unsigned char u8C)
{
	if ((u8C >= (unsigned char)'A' && u8C <= (unsigned char)'Z') ||
	    (u8C >= (unsigned char)'a' && u8C <= (unsigned char)'z') ||
	    u8C == (unsigned char)'_') {
		return 1;
	}
	return 0;
}

static int
b3046_is_body(unsigned char u8C)
{
	if (b3046_is_start(u8C)) {
		return 1;
	}
	if (u8C >= (unsigned char)'0' && u8C <= (unsigned char)'9') {
		return 1;
	}
	return 0;
}

/* 1u if szKey is a non-empty POSIX-ish env name within scan budget. */
static uint32_t
b3046_key_ok(const char *szKey)
{
	size_t i;

	if (szKey == NULL || szKey[0] == '\0') {
		return 0u;
	}
	if (!b3046_is_start((unsigned char)szKey[0])) {
		return 0u;
	}
	i = 1u;
	while (i < B3046_SCAN_MAX && szKey[i] != '\0') {
		if (!b3046_is_body((unsigned char)szKey[i])) {
			return 0u;
		}
		i++;
	}
	/* Over-budget without NUL: reject (unbounded / truncated). */
	if (i >= B3046_SCAN_MAX && szKey[i] != '\0') {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_env_key_ok_u - 1 if key is a valid env variable name.
 *
 * key: NUL-terminated candidate name (may be NULL)
 *
 * Does not call libc. No parent wires.
 */
uint32_t
gj_env_key_ok_u(const char *szKey)
{
	(void)NULL;
	return b3046_key_ok(szKey);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_env_key_ok_u(const char *szKey)
    __attribute__((alias("gj_env_key_ok_u")));
