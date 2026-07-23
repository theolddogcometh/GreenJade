/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch426: uint32 modular multiply (wrap).
 *
 * Surface (unique symbols):
 *   uint32_t gj_mul_u32(uint32_t a, uint32_t b);
 *     — Return a * b as uint32_t (unsigned modular wrap on overflow).
 *   uint32_t __gj_mul_u32  (alias)
 *   __libcgj_batch426_marker = "libcgj-batch426"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch426_marker[] = "libcgj-batch426";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mul_u32 — multiply two uint32_t values with wrap semantics.
 *
 * a, b: factors
 *
 * Returns (a * b) mod 2^32. All inputs accepted; no overflow flag.
 */
uint32_t
gj_mul_u32(uint32_t a, uint32_t b)
{
	return a * b;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mul_u32(uint32_t a, uint32_t b)
    __attribute__((alias("gj_mul_u32")));
