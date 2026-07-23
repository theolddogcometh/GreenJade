/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1506: bit writer put (LSB-first into u64 hold).
 *
 * Surface (unique symbols):
 *   typedef struct {
 *       uint64_t bitbuf;   — pending bits (LSB-first fill)
 *       unsigned nfill;    — valid bit count in bitbuf (0..63)
 *       uint8_t *out;      — output cursor base
 *       size_t out_cap;    — capacity of out[]
 *       size_t out_pos;    — bytes written so far
 *   } gj_bitwriter_t;      — local typedef (this TU only)
 *
 *   int gj_bitwriter_put(gj_bitwriter_t *bw, uint64_t bits, unsigned nbits);
 *     — Append the low nbits of bits into bw (LSB-first stream order).
 *       Emits full bytes into out[] as they become available.
 *       nbits must be in 0..56 (fits with residual fill in a u64).
 *       Returns 0 on success, -1 on error (NULL bw/out, bad nbits,
 *       insufficient out_cap).
 *   int __gj_bitwriter_put  (alias)
 *   __libcgj_batch1506_marker = "libcgj-batch1506"
 *
 * Flush remaining partial byte via gj_bitwriter_flush (batch1507).
 * Pair with gj_bitreader_get (batch1508). Local typedef — no shared header.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1506_marker[] = "libcgj-batch1506";

/* ---- local state (plain struct typedef) -------------------------------- */

typedef struct {
	uint64_t bitbuf;
	unsigned nfill;
	uint8_t *out;
	size_t out_cap;
	size_t out_pos;
} gj_bitwriter_t;

/* Max bits accepted in one put (leaves room for residual nfill < 8). */
#define B1506_NBITS_MAX 56u

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Emit all complete bytes currently held in bitbuf (nfill >= 8).
 * Returns 0 on success, -1 if out buffer is full.
 */
static int
b1506_emit_bytes(gj_bitwriter_t *pBw)
{
	while (pBw->nfill >= 8u) {
		if (pBw->out_pos >= pBw->out_cap) {
			return -1;
		}
		pBw->out[pBw->out_pos] = (uint8_t)(pBw->bitbuf & 0xFFu);
		pBw->out_pos++;
		pBw->bitbuf >>= 8;
		pBw->nfill -= 8u;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitwriter_put — append nbits low bits of bits into the bit buffer.
 *
 * Stream order is LSB-first within each emitted byte (DEFLATE-shaped).
 * bits above bit (nbits-1) are ignored via mask.
 */
int
gj_bitwriter_put(gj_bitwriter_t *pBw, uint64_t u64Bits, unsigned nBits)
{
	uint64_t u64Mask;
	uint64_t u64Chunk;

	if (pBw == NULL || pBw->out == NULL) {
		return -1;
	}
	if (nBits > B1506_NBITS_MAX) {
		return -1;
	}
	if (nBits == 0u) {
		return 0;
	}
	/* Residual fill is always < 8 after emit; nBits <= 56 ⇒ total < 64. */
	if (pBw->nfill > 63u - nBits) {
		return -1;
	}

	/* nBits in 1..56 — shift is well-defined for uint64_t. */
	u64Mask = ((uint64_t)1 << nBits) - 1u;
	u64Chunk = u64Bits & u64Mask;
	pBw->bitbuf |= (u64Chunk << pBw->nfill);
	pBw->nfill += nBits;

	return b1506_emit_bytes(pBw);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bitwriter_put(gj_bitwriter_t *pBw, uint64_t u64Bits, unsigned nBits)
    __attribute__((alias("gj_bitwriter_put")));
