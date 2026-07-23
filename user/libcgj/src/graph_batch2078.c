/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2078: pure ASCII hex digit-string predicate.
 *
 * Surface (unique symbols):
 *   int gj_str_is_hex_ascii(const char *s);
 *     — Return 1 if s is a non-empty string of ASCII hex digits
 *       [0-9a-fA-F]+ only (full string). No "0x" / "0X" prefix accepted.
 *       NULL / empty / any non-hex octet → 0.
 *   int __gj_str_is_hex_ascii  (alias)
 *   __libcgj_batch2078_marker = "libcgj-batch2078"
 *
 * Str encode exclusive wave (2071–2080). Distinct from gj_is_hex_str
 * (batch450, optional 0x prefix) — pure hex only; avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2078_marker[] = "libcgj-batch2078";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2078_is_hex(unsigned char u8C)
{
	if (u8C >= (unsigned char)'0' && u8C <= (unsigned char)'9') {
		return 1;
	}
	if (u8C >= (unsigned char)'a' && u8C <= (unsigned char)'f') {
		return 1;
	}
	if (u8C >= (unsigned char)'A' && u8C <= (unsigned char)'F') {
		return 1;
	}
	return 0;
}

static int
b2078_is_hex_str(const char *sz)
{
	const char *p;
	size_t cDig;

	p = sz;
	cDig = 0u;
	while (*p != '\0') {
		if (!b2078_is_hex((unsigned char)*p)) {
			return 0;
		}
		p++;
		cDig++;
	}
	if (cDig == 0u) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_is_hex_ascii — 1 if s is nonempty pure ASCII hex [0-9a-fA-F]+.
 *
 * Examples:
 *   "0" / "ff" / "FF" / "DeadBeef" → 1
 *   NULL / "" / "0x1" / "1g" / " 1" → 0
 */
int
gj_str_is_hex_ascii(const char *s)
{
	(void)NULL;
	if (s == NULL || s[0] == '\0') {
		return 0;
	}
	return b2078_is_hex_str(s);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_is_hex_ascii(const char *s)
    __attribute__((alias("gj_str_is_hex_ascii")));
