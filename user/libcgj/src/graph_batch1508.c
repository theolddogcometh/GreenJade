/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1508: bit reader get (LSB-first from u64 hold).
 *
 * Surface (unique symbols):
 *   typedef struct {
 *       uint64_t bitbuf;     — pending bits (LSB-first)
 *       unsigned nfill;      — valid bit count in bitbuf (0..64)
 *       const uint8_t *in;   — input base
 *       size_t in_len;       — total input bytes
 *       size_t in_pos;       — next byte index into in[]
 *   } gj_bitreader_t;        — local typedef (this TU only)
 *
 *   int gj_bitreader_get(gj_bitreader_t *br, unsigned nbits,
 *                        uint64_t *out_bits);
 *     — Extract nbits LSB-first into *out_bits (high bits zero).
 *       Refills bitbuf from in[] as needed (up to 8 bytes at a time).
 *       nbits must be in 0..56.
 *       Returns 0 on success, -1 on error (NULL args, bad nbits,
 *       insufficient input).
 *   int __gj_bitreader_get  (alias)
 *   __libcgj_batch1508_marker = "libcgj-batch1508"
 *
 * Stream order matches gj_bitwriter_put / flush (batch1506/1507).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1508_marker[] = "libcgj-batch1508";

/* ---- local state (plain struct typedef) -------------------------------- */

typedef struct {
	uint64_t bitbuf;
	unsigned nfill;
	const uint8_t *in;
	size_t in_len;
	size_t in_pos;
} gj_bitreader_t;

#define B1508_NBITS_MAX 56u

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Pull whole bytes from in[] into bitbuf until nfill >= nNeed or EOF.
 * Returns 0 if nfill >= nNeed, -1 if input exhausted early.
 */
static int
b1508_refill(gj_bitreader_t *pBr, unsigned nNeed)
{
	while (pBr->nfill < nNeed) {
		if (pBr->in_pos >= pBr->in_len) {
			return -1;
		}
		/* Room for at least one more byte in the 64-bit hold. */
		if (pBr->nfill > 56u) {
			return -1;
		}
		pBr->bitbuf |= ((uint64_t)pBr->in[pBr->in_pos]) << pBr->nfill;
		pBr->in_pos++;
		pBr->nfill += 8u;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitreader_get — pull nbits from the LSB-first bit stream into *out.
 */
int
gj_bitreader_get(gj_bitreader_t *pBr, unsigned nBits, uint64_t *pOutBits)
{
	uint64_t u64Mask;
	uint64_t u64Val;

	if (pBr == NULL || pOutBits == NULL) {
		return -1;
	}
	if (pBr->in == NULL && nBits != 0u) {
		return -1;
	}
	if (nBits > B1508_NBITS_MAX) {
		return -1;
	}
	if (nBits == 0u) {
		*pOutBits = 0u;
		return 0;
	}

	if (b1508_refill(pBr, nBits) != 0) {
		return -1;
	}

	/* nBits in 1..56 — shift is well-defined for uint64_t. */
	u64Mask = ((uint64_t)1 << nBits) - 1u;
	u64Val = pBr->bitbuf & u64Mask;
	pBr->bitbuf >>= nBits;
	pBr->nfill -= nBits;
	*pOutBits = u64Val;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bitreader_get(gj_bitreader_t *pBr, unsigned nBits, uint64_t *pOutBits)
    __attribute__((alias("gj_bitreader_get")));
