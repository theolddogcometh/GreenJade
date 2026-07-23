/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3737: dense uint32 square identity (_u).
 *
 * Surface (unique symbols):
 *   void gj_matrix_identity_u(uint32_t *m, size_t n);
 *     - Write the n x n identity into m as n*n row-major uint32 values
 *       (1 on the diagonal, 0 elsewhere). No-op if m is NULL or n is 0.
 *       No-op if n*n would overflow size_t.
 *   void __gj_matrix_identity_u  (alias)
 *   __libcgj_batch3737_marker = "libcgj-batch3737"
 *
 * Exclusive continuum CREATE-ONLY (3731-3740). Distinct from
 * gj_mat2_i32_identity / gj_mat3_identity_i32 — unique uint32 n x n
 * surface; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3737_marker[] = "libcgj-batch3737";

/* ---- freestanding helpers ---------------------------------------------- */

/* Zero then set diagonal of n x n row-major uint32 matrix. */
static void
b3737_identity(uint32_t *pM, size_t uN)
{
	size_t uElems;
	size_t uI;

	if (uN > ((size_t)-1) / uN) {
		return;
	}
	uElems = uN * uN;
	for (uI = 0u; uI < uElems; uI++) {
		pM[uI] = 0u;
	}
	for (uI = 0u; uI < uN; uI++) {
		pM[uI * uN + uI] = 1u;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_matrix_identity_u — store I_n into m (row-major uint32).
 *
 * m: output n x n matrix
 * n: dimension
 *
 * No-op on NULL / empty / overflow. No parent wires.
 */
void
gj_matrix_identity_u(uint32_t *m, size_t n)
{
	(void)NULL;
	if (m == NULL || n == 0u) {
		return;
	}
	b3737_identity(m, n);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_matrix_identity_u(uint32_t *m, size_t n)
    __attribute__((alias("gj_matrix_identity_u")));
