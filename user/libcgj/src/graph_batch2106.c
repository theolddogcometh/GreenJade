/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2106: span C0/DEL control-presence predicate.
 *
 * Surface (unique symbols):
 *   int gj_str_has_ctrl_p(const char *s, size_t n);
 *     - Return 1 if s[0..n) contains at least one C0 control octet
 *       (0x00..0x1F) or DEL (0x7F), else 0. s == NULL or n == 0 -> 0.
 *       Does not require a terminating NUL. High-bit octets are not
 *       treated as controls here.
 *   int __gj_str_has_ctrl_p  (alias)
 *   __libcgj_batch2106_marker = "libcgj-batch2106"
 *
 * Post-2100 str trim exclusive wave (2101-2110). Unique gj_str_has_ctrl_p
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2106_marker[] = "libcgj-batch2106";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2106_is_ctrl(unsigned char u8C)
{
	return (u8C <= 0x1fu || u8C == 0x7fu) ? 1 : 0;
}

static int
b2106_has_ctrl(const char *sz, size_t cb)
{
	size_t i;

	if (sz == NULL || cb == 0u) {
		return 0;
	}
	for (i = 0u; i < cb; i++) {
		if (b2106_is_ctrl((unsigned char)sz[i])) {
			return 1;
		}
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_has_ctrl_p - 1 if span s[0..n) contains any C0 control or DEL.
 *
 * s: buffer of n octets (NULL -> 0)
 * n: span length
 *
 * Early-exits on the first match. Does not call libc.
 */
int
gj_str_has_ctrl_p(const char *s, size_t n)
{
	return b2106_has_ctrl(s, n);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_has_ctrl_p(const char *s, size_t n)
    __attribute__((alias("gj_str_has_ctrl_p")));
