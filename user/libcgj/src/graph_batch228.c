/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch228: naive polynomial multiply (integer coeffs).
 *
 * Clean-room freestanding pure C — integer/pointer only (no SSE required;
 * builds under userspace -msse2). No third-party poly / NTT source was copied.
 *
 * Surface (unique symbols):
 *   void gj_poly_mul_naive(const int64_t *a, size_t na,
 *                          const int64_t *b, size_t nb,
 *                          int64_t *out);
 *     — Schoolbook product of degree-(na-1) poly a[] with degree-(nb-1)
 *       poly b[]. Result coefficients written to out[0 .. na+nb-2]
 *       (length na+nb-1). out must have room for that many int64_t.
 *     — out is zeroed then out[i+j] += a[i]*b[j] for all i,j.
 *     — NULL a/b/out or zero na/nb → no-op (when na or nb is 0 and out
 *       is non-NULL with na+nb-1==0 conceptually: still a no-op).
 *   __gj_poly_mul_naive  (alias)
 *   __libcgj_batch228_marker = "libcgj-batch228"
 *
 * Notes:
 *   - O(na*nb) multiplies; no FFT / Karatsuba (prefer simple for this batch).
 *   - Products use int64_t arithmetic; overflow wraps (two's complement).
 *   - Distinct from any radix-2 FFT stub; FFT not provided here.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch228_marker[] = "libcgj-batch228";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_poly_mul_naive — schoolbook convolution of two int64 coefficient vectors.
 *
 * a[0..na) and b[0..nb) are coefficients low-degree first.
 * On success out[k] = sum_{i+j=k} a[i]*b[j] for k in 0 .. na+nb-2.
 * Requires out to hold (na+nb-1) elements when na>0 and nb>0.
 */
void
gj_poly_mul_naive(const int64_t *a, size_t na, const int64_t *b, size_t nb,
    int64_t *out)
{
	size_t i;
	size_t j;
	size_t nOut;

	if (a == NULL || b == NULL || out == NULL) {
		return;
	}
	if (na == 0u || nb == 0u) {
		return;
	}

	nOut = na + nb - 1u;
	for (i = 0u; i < nOut; i++) {
		out[i] = 0;
	}

	for (i = 0u; i < na; i++) {
		int64_t ai = a[i];

		for (j = 0u; j < nb; j++) {
			out[i + j] += ai * b[j];
		}
	}
}

void __gj_poly_mul_naive(const int64_t *a, size_t na, const int64_t *b,
    size_t nb, int64_t *out) __attribute__((alias("gj_poly_mul_naive")));
