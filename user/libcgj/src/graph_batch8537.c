/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8537: complex multiply imag-part stub (_u).
 *
 * Surface (unique symbols):
 *   int64_t gj_cmul_im_u_8537(int32_t ar, int32_t ai, int32_t br,
 *                             int32_t bi);
 *     - Imag part of (ar+i*ai)*(br+i*bi) = ar*bi + ai*br.
 *       Products widen to int64_t (no __int128; no signed int32 mul UB).
 *   int64_t __gj_cmul_im_u_8537  (alias)
 *   __libcgj_batch8537_marker = "libcgj-batch8537"
 *
 * Exclusive continuum CREATE-ONLY (8531-8540: complex/polar id stubs —
 * integer magnitude-ish only). Unique gj_cmul_im_u_8537 surface only;
 * no multi-def. Pair with gj_cmul_re_u_8536. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8537_marker[] = "libcgj-batch8537";

/* ---- freestanding helpers ---------------------------------------------- */

static int64_t
b8537_cmul_im(int32_t i32Ar, int32_t i32Ai, int32_t i32Br, int32_t i32Bi)
{
	int64_t i64Ad;
	int64_t i64Bc;

	i64Ad = (int64_t)i32Ar * (int64_t)i32Bi;
	i64Bc = (int64_t)i32Ai * (int64_t)i32Br;
	return i64Ad + i64Bc;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cmul_im_u_8537 - imag part of integer complex multiply.
 *
 * (ar, ai) * (br, bi) → imag = ar*bi + ai*br (widened int64).
 * No parent wires.
 */
int64_t
gj_cmul_im_u_8537(int32_t i32Ar, int32_t i32Ai, int32_t i32Br,
    int32_t i32Bi)
{
	(void)NULL;
	return b8537_cmul_im(i32Ar, i32Ai, i32Br, i32Bi);
}

/* ---- underscored alias ------------------------------------------------- */

int64_t __gj_cmul_im_u_8537(int32_t i32Ar, int32_t i32Ai, int32_t i32Br,
    int32_t i32Bi)
    __attribute__((alias("gj_cmul_im_u_8537")));
