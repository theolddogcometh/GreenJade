/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3803: wrapping uint32_t addition (wrap_add_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_wrap_add_u(uint32_t a, uint32_t b);
 *     - Return a + b with modular wrap (mod 2^32). Plain unsigned
 *       wrap; not saturating. Unsigned-form exclusive surface.
 *   uint32_t __gj_u32_wrap_add_u  (alias)
 *   __libcgj_batch3803_marker = "libcgj-batch3803"
 *
 * Exclusive continuum CREATE-ONLY wave (3801-3810). Unique
 * gj_u32_wrap_add_u surface only; no multi-def. Distinct from
 * gj_u32_add_wrap_u (batch3201), gj_u32_add_wrap (batch2311), and
 * gj_u64_wrap_add (batch738). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3803_marker[] = "libcgj-batch3803";

/* ---- freestanding helpers ---------------------------------------------- */

/* Modular wrap add mod 2^32 (C unsigned arithmetic). */
static uint32_t
b3803_add(uint32_t u32A, uint32_t u32B)
{
	return u32A + u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_wrap_add_u - modular wrap addition of two uint32_t values.
 *
 * a, b: summands
 *
 * Returns (a + b) mod 2^32. C unsigned arithmetic already wraps; this
 * surface makes the wrap intent explicit for graph callers.
 * No parent wires.
 */
uint32_t
gj_u32_wrap_add_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b3803_add(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_wrap_add_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_wrap_add_u")));
