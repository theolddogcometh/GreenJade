/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3086: scale a length-n uint32_t vector by a
 * uint32_t scalar (exclusive n_u surface; modular wrap).
 *
 * Surface (unique symbols):
 *   void gj_scale_u32_n_u(const uint32_t *a, uint32_t s, uint32_t *out,
 *                         size_t n);
 *     - out[i] = a[i] * s for i in [0, n). uint32 wrap on overflow.
 *       No-op if a or out is NULL or n is 0. out may equal a (in-place OK).
 *   void __gj_scale_u32_n_u  (alias)
 *   __libcgj_batch3086_marker = "libcgj-batch3086"
 *
 * Exclusive unsigned vector reduce/scale wave (3081-3090). Unique
 * gj_scale_u32_n_u surface only; no multi-def. Distinct from
 * gj_dpi_scale_u32 (batch1905). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3086_marker[] = "libcgj-batch3086";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Element-wise scale a[0..cN) by u32S into pOut. Caller guarantees both
 * pointers non-NULL when cN > 0. out may alias a.
 */
static void
b3086_scale(const uint32_t *pA, uint32_t u32S, uint32_t *pOut, size_t cN)
{
	size_t i;

	for (i = 0u; i < cN; i++) {
		pOut[i] = pA[i] * u32S;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_scale_u32_n_u - scale length-n u32 vector by scalar (wrap mul).
 *
 * a:   input vector of length n
 * s:   scale factor
 * out: output vector of length n; may equal a
 * n:   element count
 */
void
gj_scale_u32_n_u(const uint32_t *a, uint32_t s, uint32_t *out, size_t n)
{
	(void)NULL;
	if (n == 0u || a == NULL || out == NULL) {
		return;
	}
	b3086_scale(a, s, out, n);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_scale_u32_n_u(const uint32_t *a, uint32_t s, uint32_t *out,
                        size_t n)
    __attribute__((alias("gj_scale_u32_n_u")));
