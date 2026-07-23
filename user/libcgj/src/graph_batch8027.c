/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8027: reallocarray size multiply stub.
 *
 * Surface (unique symbols):
 *   size_t gj_realloc_size_mul_u_8027(size_t nmemb, size_t size);
 *     - Return nmemb * size, or 0 if the product overflows size_t.
 *   size_t __gj_realloc_size_mul_u_8027  (alias)
 *   __libcgj_batch8027_marker = "libcgj-batch8027"
 *
 * Exclusive continuum CREATE-ONLY (8021-8030: realloc size stubs —
 * zero_size_id, min_size_id, grow_factor_id, size_is_zero, size_mul_ok,
 * size_ok, size_mul, size_grow, size_errorish, batch_id_8030).
 * Unique gj_realloc_size_mul_u_8027 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8027_marker[] = "libcgj-batch8027";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b8027_size_mul — nmemb * size; 0 on overflow (and also when either
 * factor is 0, which is a valid zero total).
 *
 * Overflow detected via size > SIZE_MAX / nmemb when nmemb != 0.
 * Note: overflow and true zero product both return 0; callers that
 * need to distinguish should use gj_realloc_size_mul_ok_u_8025 first.
 */
static size_t
b8027_size_mul(size_t uNmemb, size_t uSize)
{
if (uNmemb == 0u || uSize == 0u) {
return 0u;
}
if (uSize > (SIZE_MAX / uNmemb)) {
return 0u;
}
return uNmemb * uSize;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_realloc_size_mul_u_8027 - soft nmemb*size for reallocarray.
 *
 * nmemb: element count
 * size:  element width in bytes
 *
 * Returns the product when it fits in size_t; returns 0 on overflow
 * or when either factor is 0. Pure integer catalog; does not call
 * reallocarray(3). No parent wires.
 */
size_t
gj_realloc_size_mul_u_8027(size_t uNmemb, size_t uSize)
{
(void)NULL;
return b8027_size_mul(uNmemb, uSize);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_realloc_size_mul_u_8027(size_t uNmemb, size_t uSize)
    __attribute__((alias("gj_realloc_size_mul_u_8027")));
