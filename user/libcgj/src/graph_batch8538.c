/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8538: complex add real-part stub (_u).
 *
 * Surface (unique symbols):
 *   int32_t gj_cadd_re_u_8538(int32_t ar, int32_t br);
 *     - Real part of (ar+i*ai)+(br+i*bi) = ar + br.
 *       Sum via int64 intermediate, then truncated to int32_t (wrap
 *       semantics; no signed-add UB on the int32 path).
 *   int32_t __gj_cadd_re_u_8538  (alias)
 *   __libcgj_batch8538_marker = "libcgj-batch8538"
 *
 * Exclusive continuum CREATE-ONLY (8531-8540: complex/polar id stubs —
 * integer magnitude-ish only). Unique gj_cadd_re_u_8538 surface only;
 * no multi-def. Pair with gj_cadd_im_u_8539. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8538_marker[] = "libcgj-batch8538";

/* ---- freestanding helpers ---------------------------------------------- */

static int32_t
b8538_cadd_re(int32_t i32Ar, int32_t i32Br)
{
	return (int32_t)((int64_t)i32Ar + (int64_t)i32Br);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cadd_re_u_8538 - real part of integer complex add.
 *
 * ar, br: real parts; returns ar+br as int32 (wrap via int64 path).
 * No parent wires.
 */
int32_t
gj_cadd_re_u_8538(int32_t i32Ar, int32_t i32Br)
{
	(void)NULL;
	return b8538_cadd_re(i32Ar, i32Br);
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_cadd_re_u_8538(int32_t i32Ar, int32_t i32Br)
    __attribute__((alias("gj_cadd_re_u_8538")));
