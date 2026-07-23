/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch120: constant-time memory helpers.
 * Clean-room freestanding pure C — integer/pointer only (no libc string).
 * Builds under userspace -msse2; no SIMD required for these paths.
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   crypto_memcmp / __crypto_memcmp           → graph_batch41.c
 *   consttime_memequal / __consttime_memequal → graph_batch28.c
 *   timingsafe_bcmp / timingsafe_memcmp       → graph_batch24.c
 *   explicit_bzero / __explicit_bzero         → string.c / batch30
 *
 * This TU adds only unique symbols:
 *   int      gj_memeq_ct(const void *a, const void *b, size_t n);
 *     — 1 if equal, 0 if not; no data-dependent early exit.
 *   void     gj_memzero_s(void *p, size_t n);
 *     — secure wipe via volatile stores (not elidable).
 *   uint32_t gj_select_u32(int cond, uint32_t a, uint32_t b);
 *     — branch-free select: cond != 0 → a, else b.
 *   __gj_memeq_ct / __gj_memzero_s / __gj_select_u32  (aliases)
 *   __libcgj_batch120_marker = "libcgj-batch120"
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch120_marker[] = "libcgj-batch120";

/* ---- gj_memeq_ct -------------------------------------------------------- */

/*
 * Constant-time byte equality. Walks the full length; accumulates XOR
 * differences. Returns 1 when all bytes match (or n == 0), else 0.
 * NULL pointer with n > 0 is treated as not-equal.
 */
int
gj_memeq_ct(const void *pA, const void *pB, size_t cb)
{
	const unsigned char *a;
	const unsigned char *b;
	unsigned char uDiff;
	size_t i;

	if (cb == 0u) {
		return 1;
	}
	if (pA == NULL || pB == NULL) {
		return 0;
	}

	a = (const unsigned char *)pA;
	b = (const unsigned char *)pB;
	uDiff = 0;
	for (i = 0u; i < cb; i++) {
		uDiff = (unsigned char)(uDiff | (a[i] ^ b[i]));
	}

	/*
	 * Normalize to 1 (equal) / 0 (not) without a data-dependent
	 * early exit in the scan. Final fold is value-only.
	 * uDiff==0 → ((0-1)>>8)&1 = 1; else high bits clear → 0.
	 */
	return (int)((((unsigned)uDiff - 1u) >> 8) & 1u);
}

int __gj_memeq_ct(const void *a, const void *b, size_t n)
    __attribute__((alias("gj_memeq_ct")));

/* ---- gj_memzero_s ------------------------------------------------------- */

/*
 * Secure wipe: write zeros through a volatile pointer so the compiler
 * cannot elide the stores as dead (unlike plain memset to a buffer that
 * is about to go out of scope).
 */
void
gj_memzero_s(void *p, size_t cb)
{
	volatile unsigned char *pV;
	size_t i;

	if (p == NULL || cb == 0u) {
		return;
	}
	pV = (volatile unsigned char *)p;
	for (i = 0u; i < cb; i++) {
		pV[i] = 0;
	}
}

void __gj_memzero_s(void *p, size_t n)
    __attribute__((alias("gj_memzero_s")));

/* ---- gj_select_u32 ------------------------------------------------------ */

/*
 * Branch-free select between two uint32_t values.
 * cond != 0 → a; cond == 0 → b.
 * Mask is all-ones or zero derived from cond without a data branch on a/b.
 */
uint32_t
gj_select_u32(int nCond, uint32_t uA, uint32_t uB)
{
	uint32_t uC;
	uint32_t uMask;

	/* 0 → 0, any non-zero → 1, arithmetic only */
	uC = (uint32_t)nCond;
	uMask = (uC | (0u - uC)) >> 31;
	/* broadcast 0/1 to 0x00000000 / 0xffffffff */
	uMask = 0u - uMask;

	return (uA & uMask) | (uB & ~uMask);
}

uint32_t __gj_select_u32(int nCond, uint32_t uA, uint32_t uB)
    __attribute__((alias("gj_select_u32")));
