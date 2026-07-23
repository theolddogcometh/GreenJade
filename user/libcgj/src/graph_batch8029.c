/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8029: realloc size errorish stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_realloc_size_errorish_u_8029(size_t n);
 *     - Return 0 for any size (no size is errorish for realloc).
 *   uint32_t __gj_realloc_size_errorish_u_8029  (alias)
 *   __libcgj_batch8029_marker = "libcgj-batch8029"
 *
 * Exclusive continuum CREATE-ONLY (8021-8030: realloc size stubs —
 * zero_size_id, min_size_id, grow_factor_id, size_is_zero, size_mul_ok,
 * size_ok, size_mul, size_grow, size_errorish, batch_id_8030).
 * Unique gj_realloc_size_errorish_u_8029 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8029_marker[] = "libcgj-batch8029";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8029_size_errorish(size_t uN)
{
/* Soft inverse of size_ok: no size_t is catalog-errorish alone. */
(void)uN;
return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_realloc_size_errorish_u_8029 - 1 if size is catalog-errorish.
 *
 * n: candidate realloc size argument
 *
 * Always returns 0 (soft inverse of size_ok). Overflow of nmemb*size
 * is covered by mul_ok/size_mul stubs, not this single-arg probe.
 * Does not call realloc(3). No parent wires.
 */
uint32_t
gj_realloc_size_errorish_u_8029(size_t uN)
{
(void)NULL;
return b8029_size_errorish(uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_realloc_size_errorish_u_8029(size_t uN)
    __attribute__((alias("gj_realloc_size_errorish_u_8029")));
