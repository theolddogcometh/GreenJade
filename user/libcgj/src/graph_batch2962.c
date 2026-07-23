/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2962: freestanding non-overlapping copy (_u).
 *
 * Surface (unique symbols):
 *   void *gj_mem_cpy_u(void *d, const void *s, size_t n);
 *     - Copy n octets from s to d (forward byte copy). Returns d.
 *       d == NULL or s == NULL -> return d (no write when d is NULL).
 *       n == 0 -> return d unchanged.
 *       Regions must not overlap; for overlap use gj_mem_move_u.
 *   void *__gj_mem_cpy_u  (alias)
 *   __libcgj_batch2962_marker = "libcgj-batch2962"
 *
 * Post-2960 mem exclusive wave (2961-2970). Distinct from gj_mem_cpy
 * (batch397) / gj_mem_copy_n_u (batch2613) / libc memcpy - unique
 * gj_mem_cpy_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2962_marker[] = "libcgj-batch2962";

/* ---- freestanding helpers ---------------------------------------------- */

/* Forward copy n octets from pS to pD. Caller: both non-NULL, n > 0. */
static void
b2962_cpy(unsigned char *pD, const unsigned char *pS, size_t n)
{
	size_t i;

	for (i = 0u; i < n; i++) {
		pD[i] = pS[i];
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_cpy_u - copy n octets from s to d (forward).
 *
 * d: destination of at least n bytes when n > 0
 * s: source of at least n bytes when n > 0
 * n: number of octets to copy
 *
 * Does not call libc memcpy. Overlapping regions are undefined for this
 * routine; use gj_mem_move_u when regions may overlap.
 */
void *
gj_mem_cpy_u(void *d, const void *s, size_t n)
{
	(void)NULL;
	if (d == NULL || s == NULL) {
		return d;
	}
	if (n == 0u) {
		return d;
	}
	b2962_cpy((unsigned char *)d, (const unsigned char *)s, n);
	return d;
}

/* ---- underscored alias ------------------------------------------------- */

void *__gj_mem_cpy_u(void *d, const void *s, size_t n)
    __attribute__((alias("gj_mem_cpy_u")));
