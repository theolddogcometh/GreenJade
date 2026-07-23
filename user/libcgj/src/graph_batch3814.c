/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3814: max predicate for uint32_t (_u).
 *
 * Surface (unique symbols):
 *   int gj_u32_is_max_u(uint32_t x);
 *     - Return 1 if x == UINT32_MAX (0xffffffff), else 0.
 *   int __gj_u32_is_max_u  (alias)
 *   __libcgj_batch3814_marker = "libcgj-batch3814"
 *
 * Exclusive continuum CREATE-ONLY (3811-3820). Unique gj_u32_is_max_u
 * surface only; no multi-def. Distinct from gj_u64_is_max_u (batch3815)
 * / gj_heap_is_max_u32. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3814_marker[] = "libcgj-batch3814";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b3814_is_max(uint32_t u32X)
{
	return (u32X == UINT32_MAX) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_is_max_u - 1 if x is UINT32_MAX, else 0.
 *
 * Pure integer compare. Does not call libc. No parent wires.
 */
int
gj_u32_is_max_u(uint32_t u32X)
{
	(void)NULL;
	return b3814_is_max(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_is_max_u(uint32_t u32X)
    __attribute__((alias("gj_u32_is_max_u")));
