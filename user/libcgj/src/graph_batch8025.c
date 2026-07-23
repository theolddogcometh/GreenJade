/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8025: reallocarray size multiply-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_realloc_size_mul_ok_u_8025(size_t nmemb, size_t size);
 *     - Return 1 if nmemb * size does not overflow size_t, else 0.
 *   uint32_t __gj_realloc_size_mul_ok_u_8025  (alias)
 *   __libcgj_batch8025_marker = "libcgj-batch8025"
 *
 * Exclusive continuum CREATE-ONLY (8021-8030: realloc size stubs —
 * zero_size_id, min_size_id, grow_factor_id, size_is_zero, size_mul_ok,
 * size_ok, size_mul, size_grow, size_errorish, batch_id_8030).
 * Unique gj_realloc_size_mul_ok_u_8025 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8025_marker[] = "libcgj-batch8025";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b8025_mul_ok — 1 if nmemb * size fits in size_t.
 *
 * Zero factors are always ok. Otherwise require
 * size <= SIZE_MAX / nmemb (no __int128).
 */
static uint32_t
b8025_size_mul_ok(size_t uNmemb, size_t uSize)
{
if (uNmemb == 0u || uSize == 0u) {
return 1u;
}
if (uSize <= (SIZE_MAX / uNmemb)) {
return 1u;
}
return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_realloc_size_mul_ok_u_8025 - 1 if nmemb*size does not overflow.
 *
 * nmemb: element count (reallocarray-style)
 * size:  element width in bytes
 *
 * Returns 1 when the product fits in size_t, else 0. Soft overflow
 * check only; does not call reallocarray(3). No parent wires.
 */
uint32_t
gj_realloc_size_mul_ok_u_8025(size_t uNmemb, size_t uSize)
{
(void)NULL;
return b8025_size_mul_ok(uNmemb, uSize);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_realloc_size_mul_ok_u_8025(size_t uNmemb, size_t uSize)
    __attribute__((alias("gj_realloc_size_mul_ok_u_8025")));
