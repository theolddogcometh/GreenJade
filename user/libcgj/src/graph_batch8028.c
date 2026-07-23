/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8028: realloc size grow (double) stub.
 *
 * Surface (unique symbols):
 *   size_t gj_realloc_size_grow_u_8028(size_t old);
 *     - Soft geometric grow: 0 → 1, else old * 2; 0 on overflow.
 *   size_t __gj_realloc_size_grow_u_8028  (alias)
 *   __libcgj_batch8028_marker = "libcgj-batch8028"
 *
 * Exclusive continuum CREATE-ONLY (8021-8030: realloc size stubs —
 * zero_size_id, min_size_id, grow_factor_id, size_is_zero, size_mul_ok,
 * size_ok, size_mul, size_grow, size_errorish, batch_id_8030).
 * Unique gj_realloc_size_grow_u_8028 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8028_marker[] = "libcgj-batch8028";

/* Soft grow factor matching gj_realloc_grow_factor_id_8023. */
#define B8028_GROW_FACTOR  ((size_t)2u)
#define B8028_MIN_SIZE     ((size_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b8028_size_grow — double capacity; seed empty with min size 1.
 *
 * Overflow when old > SIZE_MAX / grow_factor → return 0.
 */
static size_t
b8028_size_grow(size_t uOld)
{
if (uOld == 0u) {
return B8028_MIN_SIZE;
}
if (uOld > (SIZE_MAX / B8028_GROW_FACTOR)) {
return 0u;
}
return uOld * B8028_GROW_FACTOR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_realloc_size_grow_u_8028 - soft next capacity after grow.
 *
 * old: current allocation / capacity size
 *
 * Returns 1 when old is 0; otherwise old * 2 when representable, else
 * 0 on overflow. Pure integer catalog; does not call realloc(3).
 * No parent wires.
 */
size_t
gj_realloc_size_grow_u_8028(size_t uOld)
{
(void)NULL;
return b8028_size_grow(uOld);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_realloc_size_grow_u_8028(size_t uOld)
    __attribute__((alias("gj_realloc_size_grow_u_8028")));
