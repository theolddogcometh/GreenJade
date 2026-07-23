/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3209: bitwise NOT of uint32_t (u-suffix).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_not_u(uint32_t x);
 *     - Return ~x (bitwise complement of all 32 bits).
 *       gj_u32_not_u(0) == UINT32_MAX.
 *       gj_u32_not_u(UINT32_MAX) == 0.
 *   uint32_t __gj_u32_not_u  (alias)
 *   __libcgj_batch3209_marker = "libcgj-batch3209"
 *
 * Exclusive continuum CREATE-ONLY wave (3201-3210). Unique
 * gj_u32_not_u surface only; no multi-def. Distinct from
 * gj_u32_neg_wrap_u (batch3207 modular additive inverse).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3209_marker[] = "libcgj-batch3209";

/* ---- freestanding helpers ---------------------------------------------- */

/* Bitwise complement of all 32 bits. */
static uint32_t
b3209_not(uint32_t u32X)
{
return (uint32_t)(~u32X);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_not_u - bitwise NOT of a uint32_t value.
 *
 * Returns ~x. Not modular negation (that is gj_u32_neg_wrap_u).
 * No parent wires.
 */
uint32_t
gj_u32_not_u(uint32_t u32X)
{
(void)NULL;
return b3209_not(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_not_u(uint32_t u32X)
    __attribute__((alias("gj_u32_not_u")));
