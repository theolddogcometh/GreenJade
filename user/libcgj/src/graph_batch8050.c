/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8050: memcpy-overlap continuum lamp + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_memcpy_overlap_continuum_ok_8050(void);
 *     - Returns 1 (soft lamp: 8041-8050 memcpy overlap stubs continuum
 *       complete / ready).
 *   uint32_t gj_batch_id_8050(void);
 *     - Returns the compile-time graph batch number for this TU (8050).
 *   void *gj_mempcpy_move_u_8050(void *dst, const void *src, size_t n);
 *     - Overlap-safe copy of n octets; returns pointer past last written
 *       octet (dst + n), or dst when n == 0 / NULL refused without write
 *       (returns dst). Soft mempcpy+memmove hybrid stub.
 *   uint32_t __gj_memcpy_overlap_continuum_ok_8050  (alias)
 *   uint32_t __gj_batch_id_8050  (alias)
 *   void *__gj_mempcpy_move_u_8050  (alias)
 *   __libcgj_batch8050_marker = "libcgj-batch8050"
 *
 * Exclusive continuum CREATE-ONLY (8041-8050: memcpy overlap stubs —
 * memcpy_fwd, memmove, overlap_p, memcpy_or_move, memcpy_strict,
 * memmove_bytes, copy_dir, memmove_left, memmove_right, continuum).
 * Unique *_8050 surfaces only; no multi-def. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8050_marker[] = "libcgj-batch8050";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b8050_fwd(unsigned char *pDst, const unsigned char *pSrc, size_t cb)
{
	size_t i;

	for (i = 0u; i < cb; i++) {
		pDst[i] = pSrc[i];
	}
}

static void
b8050_bwd(unsigned char *pDst, const unsigned char *pSrc, size_t cb)
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
 * gj_memcpy_overlap_continuum_ok_8050 - soft continuum-ready lamp.
 *
 * Always returns 1. Link-time presence tags the 8041-8050 memcpy
 * overlap stubs wave as complete. No parent wires.
 */
uint32_t
gj_memcpy_overlap_continuum_ok_8050(void)
{
	(void)NULL;
	return 1u;
}

/*
 * gj_batch_id_8050 - report this TU's graph batch number.
 *
 * Always returns 8050.
 */
uint32_t
gj_batch_id_8050(void)
{
	return 8050u;
}

/*
 * gj_mempcpy_move_u_8050 - overlap-safe copy; returns end pointer.
 *
 * dst: destination of at least n bytes (NULL with n > 0 → return dst)
 * src: source of at least n bytes (NULL with n > 0 → return dst)
 * n:   octet count
 *
 * After a successful move of n > 0 octets, returns (unsigned char *)dst
 * + n. On no-op (n == 0 or refused NULL), returns dst. Soft freestanding
 * mempcpy+memmove hybrid. No parent wires.
 */
void *
gj_mempcpy_move_u_8050(void *pDst, const void *pSrc, size_t n)
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
		return (void *)((unsigned char *)pDst + n);
	}

	pD = (unsigned char *)pDst;
	pS = (const unsigned char *)pSrc;
	uDst = (uintptr_t)pD;
	uSrc = (uintptr_t)pS;

	if (uDst > uSrc) {
		b8050_bwd(pD, pS, n);
	} else {
		b8050_fwd(pD, pS, n);
	}
	return (void *)(pD + n);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_memcpy_overlap_continuum_ok_8050(void)
    __attribute__((alias("gj_memcpy_overlap_continuum_ok_8050")));

uint32_t __gj_batch_id_8050(void)
    __attribute__((alias("gj_batch_id_8050")));

void *__gj_mempcpy_move_u_8050(void *pDst, const void *pSrc, size_t n)
    __attribute__((alias("gj_mempcpy_move_u_8050")));
