/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8024: realloc size-is-zero predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_realloc_size_is_zero_u_8024(size_t n);
 *     - Return 1 if n is zero (realloc free-or-min path), else 0.
 *   uint32_t __gj_realloc_size_is_zero_u_8024  (alias)
 *   __libcgj_batch8024_marker = "libcgj-batch8024"
 *
 * Exclusive continuum CREATE-ONLY (8021-8030: realloc size stubs —
 * zero_size_id, min_size_id, grow_factor_id, size_is_zero, size_mul_ok,
 * size_ok, size_mul, size_grow, size_errorish, batch_id_8030).
 * Unique gj_realloc_size_is_zero_u_8024 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8024_marker[] = "libcgj-batch8024";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8024_size_is_zero(size_t uN)
{
return (uN == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_realloc_size_is_zero_u_8024 - 1 if size is zero.
 *
 * n: candidate realloc size argument
 *
 * Returns 1 when n == 0, else 0. Soft predicate only; does not call
 * realloc(3). No parent wires.
 */
uint32_t
gj_realloc_size_is_zero_u_8024(size_t uN)
{
(void)NULL;
return b8024_size_is_zero(uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_realloc_size_is_zero_u_8024(size_t uN)
    __attribute__((alias("gj_realloc_size_is_zero_u_8024")));
