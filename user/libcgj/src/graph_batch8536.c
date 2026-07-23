/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8536: complex multiply real-part stub (_u).
 *
 * Surface (unique symbols):
 *   int64_t gj_cmul_re_u_8536(int32_t ar, int32_t ai, int32_t br,
 *                             int32_t bi);
 *     - Real part of (ar+i*ai)*(br+i*bi) = ar*br - ai*bi.
 *       Products widen to int64_t (no __int128; no signed int32 mul UB).
 *   int64_t __gj_cmul_re_u_8536  (alias)
 *   __libcgj_batch8536_marker = "libcgj-batch8536"
 *
 * Exclusive continuum CREATE-ONLY (8531-8540: complex/polar id stubs —
 * integer magnitude-ish only). Unique gj_cmul_re_u_8536 surface only;
 * no multi-def. Pair with gj_cmul_im_u_8537. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8536_marker[] = "libcgj-batch8536";

/* ---- freestanding helpers ---------------------------------------------- */

static int64_t
b8536_cmul_re(int32_t i32Ar, int32_t i32Ai, int32_t i32Br, int32_t i32Bi)
{
	int64_t i64Ac;
	int64_t i64Bd;

	i64Ac = (int64_t)i32Ar * (int64_t)i32Br;
	i64Bd = (int64_t)i32Ai * (int64_t)i32Bi;
	return i64Ac - i64Bd;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cmul_re_u_8536 - real part of integer complex multiply.
 *
 * (ar, ai) * (br, bi) → real = ar*br - ai*bi (widened int64).
 * No parent wires.
 */
int64_t
gj_cmul_re_u_8536(int32_t i32Ar, int32_t i32Ai, int32_t i32Br,
    int32_t i32Bi)
{
	(void)NULL;
	return b8536_cmul_re(i32Ar, i32Ai, i32Br, i32Bi);
}

/* ---- underscored alias ------------------------------------------------- */

int64_t __gj_cmul_re_u_8536(int32_t i32Ar, int32_t i32Ai, int32_t i32Br,
    int32_t i32Bi)
    __attribute__((alias("gj_cmul_re_u_8536")));
