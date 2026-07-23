/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8026: realloc size-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_realloc_size_ok_u_8026(size_t n);
 *     - Return 1 for any size_t (realloc accepts all size args).
 *   uint32_t __gj_realloc_size_ok_u_8026  (alias)
 *   __libcgj_batch8026_marker = "libcgj-batch8026"
 *
 * Exclusive continuum CREATE-ONLY (8021-8030: realloc size stubs —
 * zero_size_id, min_size_id, grow_factor_id, size_is_zero, size_mul_ok,
 * size_ok, size_mul, size_grow, size_errorish, batch_id_8030).
 * Unique gj_realloc_size_ok_u_8026 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8026_marker[] = "libcgj-batch8026";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8026_size_ok(size_t uN)
{
/* Catalog: every size_t is a valid realloc size argument. */
(void)uN;
return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_realloc_size_ok_u_8026 - 1 if size is acceptable for realloc.
 *
 * n: candidate realloc size argument
 *
 * Always returns 1 (soft catalog: all size_t values are ok, including
 * 0). Does not call realloc(3). No parent wires.
 */
uint32_t
gj_realloc_size_ok_u_8026(size_t uN)
{
(void)NULL;
return b8026_size_ok(uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_realloc_size_ok_u_8026(size_t uN)
    __attribute__((alias("gj_realloc_size_ok_u_8026")));
