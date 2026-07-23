/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2024: freestanding first non-zero octet probe.
 *
 * Surface (unique symbols):
 *   const void *gj_mem_first_nonzero(const void *p, size_t n);
 *     — Return a pointer to the first non-zero octet in p[0..n), or
 *       NULL if all octets are zero, n == 0, or p is NULL with n > 0.
 *       n == 0 → NULL.
 *   const void *__gj_mem_first_nonzero  (alias)
 *   __libcgj_batch2024_marker = "libcgj-batch2024"
 *
 * Post-2000 mem exclusive wave (2021–2030). Distinct from gj_mem_chr /
 * gj_mem_is_zero_n — unique gj_mem_first_nonzero surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2024_marker[] = "libcgj-batch2024";

/* ---- freestanding helpers ---------------------------------------------- */

/* Pointer to first non-zero octet, or NULL if none. */
static const unsigned char *
b2024_first_nz(const unsigned char *pU, size_t n)
{
	size_t i;

	for (i = 0u; i < n; i++) {
		if (pU[i] != 0u) {
			return pU + i;
		}
	}
	return NULL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_first_nonzero — pointer to first non-zero octet in p[0..n).
 *
 * p: region of at least n bytes when n > 0 (NULL with n > 0 → NULL)
 * n: number of octets to scan (0 → NULL)
 *
 * Does not call libc. Returns const view into p.
 */
const void *
gj_mem_first_nonzero(const void *p, size_t n)
{
	(void)NULL;
	if (n == 0u || p == NULL) {
		return NULL;
	}
	return (const void *)b2024_first_nz((const unsigned char *)p, n);
}

/* ---- underscored alias ------------------------------------------------- */

const void *__gj_mem_first_nonzero(const void *p, size_t n)
    __attribute__((alias("gj_mem_first_nonzero")));
