/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8022: realloc minimum non-zero size id stub.
 *
 * Surface (unique symbols):
 *   size_t gj_realloc_min_size_id_8022(void);
 *     - Return soft minimum non-zero realloc size (1).
 *   size_t __gj_realloc_min_size_id_8022  (alias)
 *   __libcgj_batch8022_marker = "libcgj-batch8022"
 *
 * Exclusive continuum CREATE-ONLY (8021-8030: realloc size stubs —
 * zero_size_id, min_size_id, grow_factor_id, size_is_zero, size_mul_ok,
 * size_ok, size_mul, size_grow, size_errorish, batch_id_8030).
 * Unique gj_realloc_min_size_id_8022 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8022_marker[] = "libcgj-batch8022";

/* Soft catalog: smallest non-zero realloc request size. */
#define B8022_MIN_SIZE  ((size_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b8022_min_size_id(void)
{
return B8022_MIN_SIZE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_realloc_min_size_id_8022 - soft minimum non-zero realloc size.
 *
 * Always returns 1. Catalog id only; does not call realloc(3).
 * No parent wires.
 */
size_t
gj_realloc_min_size_id_8022(void)
{
(void)NULL;
return b8022_min_size_id();
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_realloc_min_size_id_8022(void)
    __attribute__((alias("gj_realloc_min_size_id_8022")));
