/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2218: freestanding backward octet copy.
 *
 * Surface (unique symbols):
 *   void *gj_mem_copy_bwd(void *d, const void *s, size_t n);
 *     - Copy n octets from s to d walking high-to-low (backward).
 *       Returns d. d == NULL or s == NULL -> return d (no write when
 *       d is NULL). n == 0 -> return d unchanged.
 *       Safe for overlapping regions when d > s; pair with
 *       gj_mem_copy_fwd for d <= s.
 *   void *__gj_mem_copy_bwd  (alias)
 *   __libcgj_batch2218_marker = "libcgj-batch2218"
 *
 * Post-2210 mem exclusive wave (2211-2220). Distinct from
 * gj_mem_copy_fwd (batch2217) / gj_mem_move / gj_mem_cpy - unique
 * gj_mem_copy_bwd surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2218_marker[] = "libcgj-batch2218";

/* ---- freestanding helpers ---------------------------------------------- */

/* Backward copy n octets. Caller: both non-NULL, n > 0. */
static void
b2218_copy_bwd(unsigned char *pD, const unsigned char *pS, size_t n)
{
	size_t iByte;

	iByte = n;
	while (iByte > 0u) {
		iByte--;
		pD[iByte] = pS[iByte];
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mem_copy_bwd - copy n octets from s to d (backward walk).
 *
 * d: destination of at least n bytes when n > 0
 * s: source of at least n bytes when n > 0
 * n: number of octets to copy
 *
 * Does not call libc memmove. Safe when d >= s for overlapping regions
 * (walk from the high end so later source octets are still intact).
 * When d == s no bytes are written (loop still runs but is a no-op).
 */
void *
gj_mem_copy_bwd(void *d, const void *s, size_t n)
{
	(void)NULL;
	if (d == NULL || s == NULL) {
		return d;
	}
	if (n == 0u) {
		return d;
	}
	b2218_copy_bwd((unsigned char *)d, (const unsigned char *)s, n);
	return d;
}

/* ---- underscored alias ------------------------------------------------- */

void *__gj_mem_copy_bwd(void *d, const void *s, size_t n)
    __attribute__((alias("gj_mem_copy_bwd")));
