/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8134: strncat n>0 positive-limit stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strncat_n_pos_u_8134(size_t n);
 *     - Return 1 if n is greater than zero (positive payload limit);
 *       else 0.
 *   uint32_t __gj_strncat_n_pos_u_8134  (alias)
 *   __libcgj_batch8134_marker = "libcgj-batch8134"
 *
 * Exclusive continuum CREATE-ONLY (8131-8140: strcat/strncat stubs —
 * src_empty, dst_null, n_zero, n_pos, take, cat_need, ncat_need,
 * kind_unbounded, kind_bounded, batch_id_8140).
 * Unique gj_strncat_n_pos_u_8134 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8134_marker[] = "libcgj-batch8134";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8134_n_pos(size_t n)
{
	return (n > 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strncat_n_pos_u_8134 - 1 if strncat payload limit n is positive.
 *
 * n: maximum payload octets to take from src
 *
 * Returns 1 when n > 0, else 0. Soft catalog test; does not call
 * strncat(3). No parent wires.
 */
uint32_t
gj_strncat_n_pos_u_8134(size_t n)
{
	(void)NULL;
	return b8134_n_pos(n);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strncat_n_pos_u_8134(size_t n)
    __attribute__((alias("gj_strncat_n_pos_u_8134")));
