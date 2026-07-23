/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4539: classic 7-bit ASCII code-unit predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_u32_is_ascii_u(uint32_t x);
 *     - Return 1 if x <= 0x7f (classic 7-bit ASCII range), else 0.
 *   int __gj_u32_is_ascii_u  (alias)
 *   __libcgj_batch4539_marker = "libcgj-batch4539"
 *
 * Exclusive continuum CREATE-ONLY (4531-4540: validation unique). Unique
 * gj_u32_is_ascii_u surface only; no multi-def. Distinct from
 * gj_wctype_is_ascii (batch1026, int ch) and gj_utf8_is_ascii (batch691,
 * buffer form). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4539_marker[] = "libcgj-batch4539";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if x is in the classic ASCII range [0, 0x7f], else 0. */
static int
b4539_is_ascii(uint32_t u32X)
{
	if (u32X <= 0x7fu) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_is_ascii_u - 1 if x <= 0x7f (7-bit ASCII), else 0.
 *
 * x: code unit / scalar under test
 *
 * Single-value ASCII range check (not a buffer probe). No parent wires.
 */
int
gj_u32_is_ascii_u(uint32_t u32X)
{
	(void)NULL;
	return b4539_is_ascii(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_is_ascii_u(uint32_t u32X)
    __attribute__((alias("gj_u32_is_ascii_u")));
