/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8046: bytewise memmove with byte count out.
 *
 * Surface (unique symbols):
 *   size_t gj_memmove_bytes_u_8046(void *dst, const void *src, size_t n);
 *     - Overlap-safe copy of n octets; returns n on success. NULL with
 *       n > 0 → return 0 (no write). n == 0 → return 0.
 *   size_t __gj_memmove_bytes_u_8046  (alias)
 *   __libcgj_batch8046_marker = "libcgj-batch8046"
 *
 * Exclusive continuum CREATE-ONLY (8041-8050: memcpy overlap stubs —
 * memcpy_fwd, memmove, overlap_p, memcpy_or_move, memcpy_strict,
 * memmove_bytes, copy_dir, memmove_left, memmove_right, continuum).
 * Unique gj_memmove_bytes_u_8046 surface only; no multi-def with
 * gj_memmove_u_8042 (different return). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8046_marker[] = "libcgj-batch8046";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b8046_fwd(unsigned char *pDst, const unsigned char *pSrc, size_t cb)
{
	size_t i;

	for (i = 0u; i < cb; i++) {
		pDst[i] = pSrc[i];
	}
}

static void
b8046_bwd(unsigned char *pDst, const unsigned char *pSrc, size_t cb)
{
	size_t i;

	i = cb;
	while (i > 0u) {
		i--;
		pDst[i] = pSrc[i];
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_memmove_bytes_u_8046 - overlap-safe copy; returns bytes moved.
 *
 * dst: destination of at least n bytes (NULL with n > 0 → 0)
 * src: source of at least n bytes (NULL with n > 0 → 0)
 * n:   octet count
 *
 * Returns n after a successful move, else 0. Soft freestanding only.
 * No parent wires.
 */
size_t
gj_memmove_bytes_u_8046(void *pDst, const void *pSrc, size_t n)
{
	unsigned char *pD;
	const unsigned char *pS;
	uintptr_t uDst;
	uintptr_t uSrc;

	(void)NULL;
	if (n == 0u || pDst == NULL || pSrc == NULL) {
		return 0u;
	}
	if (pDst == pSrc) {
		return n;
	}

	pD = (unsigned char *)pDst;
	pS = (const unsigned char *)pSrc;
	uDst = (uintptr_t)pD;
	uSrc = (uintptr_t)pS;

	if (uDst > uSrc) {
		b8046_bwd(pD, pS, n);
	} else {
		b8046_fwd(pD, pS, n);
	}
	return n;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_memmove_bytes_u_8046(void *pDst, const void *pSrc, size_t n)
    __attribute__((alias("gj_memmove_bytes_u_8046")));
