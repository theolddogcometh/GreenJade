/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2311: wrapping uint32_t addition
 * (post-2310 arith exclusive surface).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_add_wrap(uint32_t a, uint32_t b);
 *     - Return a + b with modular wrap (mod 2^32). Plain unsigned
 *       wrap; not saturating.
 *   uint32_t __gj_u32_add_wrap  (alias)
 *   __libcgj_batch2311_marker = "libcgj-batch2311"
 *
 * Post-2310 wrap/ovf exclusive wave (2311-2320). Distinct from
 * gj_u64_wrap_add (batch738) - unique gj_u32_add_wrap surface only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2311_marker[] = "libcgj-batch2311";

/* ---- freestanding helpers ---------------------------------------------- */

/* Modular wrap add mod 2^32 (C unsigned arithmetic). */
static uint32_t
b2311_add(uint32_t u32A, uint32_t u32B)
{
	return u32A + u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_add_wrap - modular wrap addition of two uint32_t values.
 *
 * a, b: summands
 *
 * Returns (a + b) mod 2^32. C unsigned arithmetic already wraps; this
 * surface makes the wrap intent explicit for graph callers.
 */
uint32_t
gj_u32_add_wrap(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b2311_add(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_add_wrap(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_add_wrap")));
