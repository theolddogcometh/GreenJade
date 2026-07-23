/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8023: realloc grow-factor id stub.
 *
 * Surface (unique symbols):
 *   size_t gj_realloc_grow_factor_id_8023(void);
 *     - Return soft geometric grow factor (2).
 *   size_t __gj_realloc_grow_factor_id_8023  (alias)
 *   __libcgj_batch8023_marker = "libcgj-batch8023"
 *
 * Exclusive continuum CREATE-ONLY (8021-8030: realloc size stubs —
 * zero_size_id, min_size_id, grow_factor_id, size_is_zero, size_mul_ok,
 * size_ok, size_mul, size_grow, size_errorish, batch_id_8030).
 * Unique gj_realloc_grow_factor_id_8023 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8023_marker[] = "libcgj-batch8023";

/* Soft catalog: double-on-grow policy for capacity expansion. */
#define B8023_GROW_FACTOR  ((size_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b8023_grow_factor_id(void)
{
return B8023_GROW_FACTOR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_realloc_grow_factor_id_8023 - soft realloc grow factor constant.
 *
 * Always returns 2. Catalog id only; does not call realloc(3).
 * No parent wires.
 */
size_t
gj_realloc_grow_factor_id_8023(void)
{
(void)NULL;
return b8023_grow_factor_id();
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_realloc_grow_factor_id_8023(void)
    __attribute__((alias("gj_realloc_grow_factor_id_8023")));
