/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3211: bitwise NOT of a uint64_t (u-suffix).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_not_u(uint64_t x);
 *     - Return the ones-complement of x (~x). Pure integer; no
 *       builtins.
 *   uint64_t __gj_u64_not_u  (alias)
 *   __libcgj_batch3211_marker = "libcgj-batch3211"
 *
 * Milestone 3220 exclusive continuum CREATE-ONLY (3211-3220). Unique
 * gj_u64_not_u surface only; no multi-def. Distinct from
 * gj_bitmap_not_u (batch2862). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3211_marker[] = "libcgj-batch3211";

/* ---- freestanding helpers ---------------------------------------------- */

/* Ones-complement of a 64-bit word. */
static uint64_t
b3211_not(uint64_t u64X)
{
return ~u64X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_not_u - bitwise NOT of a uint64_t.
 *
 * x: input word
 *
 * Returns ~x. Does not call libc. No parent wires.
 */
uint64_t
gj_u64_not_u(uint64_t u64X)
{
(void)NULL;
return b3211_not(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_not_u(uint64_t u64X)
    __attribute__((alias("gj_u64_not_u")));
