/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch287: pack/unpack bits into a byte buffer.
 *
 * Surface (unique symbols):
 *   void gj_bitpack_init(size_t *bitpos);
 *     — Reset *bitpos to 0. NULL bitpos is a no-op.
 *   int  gj_bitpack_put(uint8_t *buf, size_t cap_bits, size_t *bitpos,
 *                       uint32_t val, unsigned nbits);
 *     — Append the low nbits of val into buf starting at *bitpos.
 *       Value bits are written MSB-of-field first; within each byte the
 *       first stream bit occupies bit 7 (MSB-first within byte).
 *       Advances *bitpos by nbits on success. Returns 0, or -1 on error.
 *   int  gj_bitpack_get(const uint8_t *buf, size_t cap_bits, size_t *bitpos,
 *                       uint32_t *val, unsigned nbits);
 *     — Read nbits into *val (low nbits, MSB-of-field first) from *bitpos.
 *       Same MSB-first-within-byte layout as put. Advances *bitpos by nbits
 *       on success. Returns 0, or -1 on error.
 *   __gj_bitpack_init / __gj_bitpack_put / __gj_bitpack_get  (aliases)
 *   __libcgj_batch287_marker = "libcgj-batch287"
 *
 * Layout (MSB-first within byte):
 *   Stream bit index i maps to byte buf[i/8], bit position (7 - (i%8)).
 *   Example: put(val=0b101, nbits=3) into a zeroed buffer yields 0xA0.
 *
 * Errors (put/get → -1; bitpos unchanged on failure):
 *   NULL buf / bitpos; NULL val (get); nbits > 32; *bitpos + nbits > cap_bits;
 *   nbits > 0 with insufficient remaining capacity.
 *   nbits == 0 is a success no-op (get sets *val = 0 when val non-NULL).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch287_marker[] = "libcgj-batch287";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b287_room — true if nbits fit starting at iBit under cBitsCap.
 * Rejects iBit past capacity and overflow of iBit + nbits.
 */
static int
b287_room(size_t cBitsCap, size_t iBit, unsigned nBits)
{
	size_t cNeed;

	if (nBits > 32u) {
		return 0;
	}
	cNeed = (size_t)nBits;
	if (iBit > cBitsCap) {
		return 0;
	}
	if (cBitsCap - iBit < cNeed) {
		return 0;
	}
	return 1;
}

/*
 * b287_put1 — write one stream bit at absolute index iBit (MSB-first byte).
 * Does not range-check; caller guarantees iBit < capacity.
 */
static void
b287_put1(uint8_t *pBuf, size_t iBit, unsigned uBit)
{
	size_t iByte = iBit >> 3;
	unsigned uShift = 7u - (unsigned)(iBit & 7u);
	uint8_t uMask = (uint8_t)(1u << uShift);

	if (uBit != 0u) {
		pBuf[iByte] = (uint8_t)(pBuf[iByte] | uMask);
	} else {
		pBuf[iByte] = (uint8_t)(pBuf[iByte] & (uint8_t)~uMask);
	}
}

/*
 * b287_get1 — read one stream bit at absolute index iBit (MSB-first byte).
 */
static unsigned
b287_get1(const uint8_t *pBuf, size_t iBit)
{
	size_t iByte = iBit >> 3;
	unsigned uShift = 7u - (unsigned)(iBit & 7u);

	return (unsigned)((pBuf[iByte] >> uShift) & 1u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitpack_init — reset bit cursor to the start of the buffer.
 */
void
gj_bitpack_init(size_t *bitpos)
{
	if (bitpos == NULL) {
		return;
	}
	*bitpos = 0u;
}

/*
 * gj_bitpack_put — append nbits of val (field MSB first) at *bitpos.
 *
 * Only the low nbits of val are used. Within each byte the first packed
 * stream bit is stored in bit 7. On success *bitpos advances by nbits.
 */
int
gj_bitpack_put(uint8_t *buf, size_t cap_bits, size_t *bitpos, uint32_t val,
    unsigned nbits)
{
	size_t iPos;
	unsigned iBit;

	if (bitpos == NULL) {
		return -1;
	}
	if (nbits == 0u) {
		return 0;
	}
	if (buf == NULL) {
		return -1;
	}
	if (!b287_room(cap_bits, *bitpos, nbits)) {
		return -1;
	}

	iPos = *bitpos;
	/* Field MSB first: bit (nbits-1) of val is written first. */
	for (iBit = nbits; iBit > 0u; iBit--) {
		unsigned uBit = (unsigned)((val >> (iBit - 1u)) & 1u);

		b287_put1(buf, iPos, uBit);
		iPos++;
	}
	*bitpos = iPos;
	return 0;
}

/*
 * gj_bitpack_get — read nbits into *val (field MSB first) from *bitpos.
 *
 * *val receives the field in its low nbits (upper bits cleared). Layout
 * matches gj_bitpack_put. On success *bitpos advances by nbits.
 */
int
gj_bitpack_get(const uint8_t *buf, size_t cap_bits, size_t *bitpos,
    uint32_t *val, unsigned nbits)
{
	size_t iPos;
	unsigned iBit;
	uint32_t uOut;

	if (bitpos == NULL || val == NULL) {
		return -1;
	}
	if (nbits == 0u) {
		*val = 0u;
		return 0;
	}
	if (buf == NULL) {
		return -1;
	}
	if (!b287_room(cap_bits, *bitpos, nbits)) {
		return -1;
	}

	iPos = *bitpos;
	uOut = 0u;
	/* Reconstruct field with MSB first (matches put). */
	for (iBit = 0u; iBit < nbits; iBit++) {
		uOut = (uOut << 1) | (uint32_t)b287_get1(buf, iPos);
		iPos++;
	}
	*val = uOut;
	*bitpos = iPos;
	return 0;
}

void __gj_bitpack_init(size_t *bitpos)
    __attribute__((alias("gj_bitpack_init")));
int __gj_bitpack_put(uint8_t *buf, size_t cap_bits, size_t *bitpos,
    uint32_t val, unsigned nbits)
    __attribute__((alias("gj_bitpack_put")));
int __gj_bitpack_get(const uint8_t *buf, size_t cap_bits, size_t *bitpos,
    uint32_t *val, unsigned nbits)
    __attribute__((alias("gj_bitpack_get")));
