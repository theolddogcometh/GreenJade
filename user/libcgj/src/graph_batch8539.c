/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8539: complex add imag-part stub (_u).
 *
 * Surface (unique symbols):
 *   int32_t gj_cadd_im_u_8539(int32_t ai, int32_t bi);
 *     - Imag part of (ar+i*ai)+(br+i*bi) = ai + bi.
 *       Sum via int64 intermediate, then truncated to int32_t (wrap
 *       semantics; no signed-add UB on the int32 path).
 *   int32_t __gj_cadd_im_u_8539  (alias)
 *   __libcgj_batch8539_marker = "libcgj-batch8539"
 *
 * Exclusive continuum CREATE-ONLY (8531-8540: complex/polar id stubs —
 * integer magnitude-ish only). Unique gj_cadd_im_u_8539 surface only;
 * no multi-def. Pair with gj_cadd_re_u_8538. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8539_marker[] = "libcgj-batch8539";

/* ---- freestanding helpers ---------------------------------------------- */

static int32_t
b8539_cadd_im(int32_t i32Ai, int32_t i32Bi)
{
	return (int32_t)((int64_t)i32Ai + (int64_t)i32Bi);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cadd_im_u_8539 - imag part of integer complex add.
 *
 * ai, bi: imag parts; returns ai+bi as int32 (wrap via int64 path).
 * No parent wires.
 */
int32_t
gj_cadd_im_u_8539(int32_t i32Ai, int32_t i32Bi)
{
	(void)NULL;
	return b8539_cadd_im(i32Ai, i32Bi);
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_cadd_im_u_8539(int32_t i32Ai, int32_t i32Bi)
    __attribute__((alias("gj_cadd_im_u_8539")));
