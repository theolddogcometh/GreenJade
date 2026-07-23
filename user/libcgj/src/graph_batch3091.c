/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3091: length-n uint32 vector subtract.
 *
 * Surface (unique symbols):
 *   void gj_sub_vec_u32_n_u(const uint32_t *a, const uint32_t *b,
 *                           uint32_t *out, size_t n);
 *     - out[i] = a[i] - b[i] for i in [0, n). uint32 wrap on underflow.
 *       No-op if any pointer is NULL or n is 0. out may equal a or b
 *       (in-place OK). Unsigned element form.
 *   void __gj_sub_vec_u32_n_u  (alias)
 *   __libcgj_batch3091_marker = "libcgj-batch3091"
 *
 * Milestone 3100 exclusive continuum CREATE-ONLY (3091-3100). Unique
 * gj_sub_vec_u32_n_u surface only; no multi-def. Distinct from
 * gj_vec_add_i32 (batch445 signed add), gj_sat_sub_u64 (batch377
 * scalar), and gj_sub_vec_u64_n_u (batch3092). No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3091_marker[] = "libcgj-batch3091";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b3091_sub_u32(const uint32_t *pA, const uint32_t *pB, uint32_t *pOut,
              size_t n)
{
	size_t i;

	if (pA == NULL || pB == NULL || pOut == NULL || n == 0u) {
		return;
	}

	for (i = 0u; i < n; i++) {
		pOut[i] = pA[i] - pB[i];
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sub_vec_u32_n_u - element-wise uint32 vector subtract of length n.
 *
 * a / b: input vectors of length n
 * out:   output vector of length n; may equal a or b
 * n:     element count
 *
 * Wrapping subtract (mod 2^32). Does not call libc. No parent wires.
 */
void
gj_sub_vec_u32_n_u(const uint32_t *pA, const uint32_t *pB, uint32_t *pOut,
                   size_t n)
{
	(void)NULL;
	b3091_sub_u32(pA, pB, pOut, n);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_sub_vec_u32_n_u(const uint32_t *pA, const uint32_t *pB,
                          uint32_t *pOut, size_t n)
    __attribute__((alias("gj_sub_vec_u32_n_u")));
