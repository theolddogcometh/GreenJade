/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8042: overlap-safe memmove stub.
 *
 * Surface (unique symbols):
 *   void *gj_memmove_u_8042(void *dst, const void *src, size_t n);
 *     - Copy n octets from src to dst; regions may overlap. Chooses
 *       forward or backward direction from uintptr order. NULL with
 *       n > 0 → return dst without write; n == 0 → return dst.
 *   void *__gj_memmove_u_8042  (alias)
 *   __libcgj_batch8042_marker = "libcgj-batch8042"
 *
 * Exclusive continuum CREATE-ONLY (8041-8050: memcpy overlap stubs —
 * memcpy_fwd, memmove, overlap_p, memcpy_or_move, memcpy_strict,
 * memmove_bytes, copy_dir, memmove_left, memmove_right, continuum).
 * Unique gj_memmove_u_8042 surface only; no multi-def with libc
 * memmove. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8042_marker[] = "libcgj-batch8042";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b8042_fwd(unsigned char *pDst, const unsigned char *pSrc, size_t cb)
{
	size_t i;

	for (i = 0u; i < cb; i++) {
		pDst[i] = pSrc[i];
	}
}

static void
b8042_bwd(unsigned char *pDst, const unsigned char *pSrc, size_t cb)
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
 * gj_memmove_u_8042 - overlap-safe memmove stub.
 *
 * dst: destination of at least n bytes (NULL with n > 0 → no-op)
 * src: source of at least n bytes (NULL with n > 0 → no-op)
 * n:   octet count
 *
 * If dst address is greater than src, copies backward; else forward.
 * Returns dst. Soft freestanding only; does not call libc memmove.
 * No parent wires.
 */
void *
gj_memmove_u_8042(void *pDst, const void *pSrc, size_t n)
{
	unsigned char *pD;
	const unsigned char *pS;
	uintptr_t uDst;
	uintptr_t uSrc;

	(void)NULL;
	if (n == 0u || pDst == NULL || pSrc == NULL) {
		return pDst;
	}
	if (pDst == pSrc) {
		return pDst;
	}

	pD = (unsigned char *)pDst;
	pS = (const unsigned char *)pSrc;
	uDst = (uintptr_t)pD;
	uSrc = (uintptr_t)pS;

	if (uDst > uSrc) {
		b8042_bwd(pD, pS, n);
	} else {
		b8042_fwd(pD, pS, n);
	}
	return pDst;
}

/* ---- underscored alias ------------------------------------------------- */

void *__gj_memmove_u_8042(void *pDst, const void *pSrc, size_t n)
    __attribute__((alias("gj_memmove_u_8042")));
