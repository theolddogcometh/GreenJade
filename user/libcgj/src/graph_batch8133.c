/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8133: strncat n==0 no-op stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strncat_n_zero_u_8133(size_t n);
 *     - Return 1 if n is 0 (strncat appends nothing and still needs a
 *       terminating NUL write policy left to the caller); else 0.
 *   uint32_t __gj_strncat_n_zero_u_8133  (alias)
 *   __libcgj_batch8133_marker = "libcgj-batch8133"
 *
 * Exclusive continuum CREATE-ONLY (8131-8140: strcat/strncat stubs —
 * src_empty, dst_null, n_zero, n_pos, take, cat_need, ncat_need,
 * kind_unbounded, kind_bounded, batch_id_8140).
 * Unique gj_strncat_n_zero_u_8133 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8133_marker[] = "libcgj-batch8133";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8133_n_zero(size_t n)
{
	return (n == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strncat_n_zero_u_8133 - 1 if strncat payload limit n is zero.
 *
 * n: maximum payload octets to take from src
 *
 * Returns 1 when n == 0 (no-op payload), else 0. Soft catalog test; does
 * not call strncat(3). No parent wires.
 */
uint32_t
gj_strncat_n_zero_u_8133(size_t n)
{
	(void)NULL;
	return b8133_n_zero(n);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strncat_n_zero_u_8133(size_t n)
    __attribute__((alias("gj_strncat_n_zero_u_8133")));
