/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2963: freestanding overlap-safe move (_u).
 *
 * Surface (unique symbols):
 *   void *gj_mem_move_u(void *d, const void *s, size_t n);
 *     - Copy n octets from s to d, correct for overlapping regions.
 *       Returns d. d == NULL or s == NULL -> return d. n == 0 -> d.
 *       If d < s, copy forward; if d > s, copy backward.
 *   void *__gj_mem_move_u  (alias)
 *   __libcgj_batch2963_marker = "libcgj-batch2963"
 *
 * Post-2960 mem exclusive wave (2961-2970). Distinct from gj_mem_move
 * (batch398) / gj_mem_move_n_u (batch2614) / libc memmove - unique
 * gj_mem_move_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2963_marker[] = "libcgj-batch2963";

/* ---- freestanding helpers ---------------------------------------------- */

/* Overlap-safe copy n octets from pS to pD. Caller: both non-NULL, n > 0. */
static void
b2963_move(unsigned char *pD, const unsigned char *pS, size_t n)
{
	size_t i;

	if (pD == pS) {
		return;
	}
	if (pD < pS) {
		for (i = 0u; i < n; i++) {
			pD[i] = pS[i];
		}
	} else {
		i = n;
		while (i > 0u) {
			i--;
			pD[i] = pS[i];
		}
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_move_u - overlap-safe octet copy from s to d.
 *
 * d: destination of at least n bytes when n > 0
 * s: source of at least n bytes when n > 0
 * n: number of octets to move
 *
 * Does not call libc memmove. When d == s, no bytes are written.
 */
void *
gj_mem_move_u(void *d, const void *s, size_t n)
{
	(void)NULL;
	if (d == NULL || s == NULL) {
		return d;
	}
	if (n == 0u) {
		return d;
	}
	b2963_move((unsigned char *)d, (const unsigned char *)s, n);
	return d;
}

/* ---- underscored alias ------------------------------------------------- */

void *__gj_mem_move_u(void *d, const void *s, size_t n)
    __attribute__((alias("gj_mem_move_u")));
