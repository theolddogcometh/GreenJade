/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3088: element-wise add of two length-n
 * uint32_t vectors (exclusive n_u surface; modular wrap).
 *
 * Surface (unique symbols):
 *   void gj_add_vec_u32_n_u(const uint32_t *a, const uint32_t *b,
 *                           uint32_t *out, size_t n);
 *     - out[i] = a[i] + b[i] for i in [0, n). uint32 wrap on overflow.
 *       No-op if any pointer is NULL or n is 0. out may equal a or b
 *       (in-place OK).
 *   void __gj_add_vec_u32_n_u  (alias)
 *   __libcgj_batch3088_marker = "libcgj-batch3088"
 *
 * Exclusive unsigned vector reduce/scale wave (3081-3090). Distinct from
 * gj_vec_add_i32 (batch445 signed) and gj_vec2_i32_add (batch1457) —
 * unique gj_add_vec_u32_n_u name; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3088_marker[] = "libcgj-batch3088";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Element-wise sum a[0..cN) + b[0..cN) into pOut. Caller guarantees all
 * three pointers non-NULL when cN > 0. out may alias a or b.
 */
static void
b3088_add(const uint32_t *pA, const uint32_t *pB, uint32_t *pOut, size_t cN)
{
	size_t i;

	for (i = 0u; i < cN; i++) {
		pOut[i] = pA[i] + pB[i];
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_add_vec_u32_n_u - element-wise uint32 vector sum of length n.
 *
 * a / b: input vectors of length n
 * out:   output vector of length n; may equal a or b
 * n:     element count
 */
void
gj_add_vec_u32_n_u(const uint32_t *a, const uint32_t *b, uint32_t *out,
                   size_t n)
{
	(void)NULL;
	if (n == 0u || a == NULL || b == NULL || out == NULL) {
		return;
	}
	b3088_add(a, b, out, n);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_add_vec_u32_n_u(const uint32_t *a, const uint32_t *b, uint32_t *out,
                          size_t n)
    __attribute__((alias("gj_add_vec_u32_n_u")));
