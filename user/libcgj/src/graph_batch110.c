/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch110: minimal JPEG SOI/EOI/APP0 scan helpers.
 * Clean-room freestanding pure C — integer/pointer only (no full decode).
 * No third-party JPEG source was copied. Compiles freestanding with -msse2.
 *
 * Surface:
 *   int gj_jpeg_is_jpeg(const void *buf, size_t len);
 *     Return 1 if buf starts with SOI (0xFFD8), else 0.
 *   int gj_jpeg_find_marker(const void *buf, size_t len, unsigned marker,
 *                           size_t *off);
 *     Scan for first marker whose type byte equals (marker & 0xFF).
 *     On success writes *off = offset of the 0xFF of that marker and
 *     returns 0; returns -1 if absent / invalid / truncated.
 *   int gj_jpeg_dimensions(const void *buf, size_t len, unsigned *w,
 *                          unsigned *h);
 *     Scan for first SOF0 (0xC0) or SOF2 (0xC2); write big-endian height
 *     and width into *h / *w. Returns 0 on success, -1 on error.
 *   __gj_jpeg_is_jpeg / __gj_jpeg_find_marker / __gj_jpeg_dimensions
 *   __libcgj_batch110_marker = "libcgj-batch110"
 *
 * Marker layout (ITU-T T.81 / JFIF subset):
 *   SOI=0xD8 EOI=0xD9 APP0=0xE0 SOF0=0xC0 SOF2=0xC2 SOS=0xDA
 *   Standalone (no length): TEM 0x01, RST0-7 0xD0-0xD7, SOI, EOI.
 *   All other markers: 0xFF type, then BE u16 length (includes itself).
 *   Inside entropy data, 0xFF00 is a stuffed FF, not a marker.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch110_marker[] = "libcgj-batch110";

/* ---- JPEG marker type bytes -------------------------------------------- */

#define B110_SOI   0xD8u
#define B110_EOI   0xD9u
#define B110_SOF0  0xC0u
#define B110_SOF2  0xC2u
#define B110_SOS   0xDAu
#define B110_TEM   0x01u
#define B110_RST0  0xD0u
#define B110_RST7  0xD7u
#define B110_APP0  0xE0u

/* ---- freestanding helpers ---------------------------------------------- */

/* Markers that carry no length field (standalone). */
static int
b110_is_standalone(unsigned uType)
{
	if (uType == B110_TEM) {
		return 1;
	}
	if (uType == B110_SOI || uType == B110_EOI) {
		return 1;
	}
	if (uType >= B110_RST0 && uType <= B110_RST7) {
		return 1;
	}
	return 0;
}

/*
 * Read big-endian u16 at p[0..1]. Caller guarantees 2 bytes available.
 */
static unsigned
b110_be16(const unsigned char *p)
{
	return ((unsigned)p[0] << 8) | (unsigned)p[1];
}

/*
 * Advance past one marker segment starting at *pIdx (must point at 0xFF
 * of a marker that is not 0xFF00 stuffing).
 *
 * On entry: pBuf[*pIdx] == 0xFF and *pIdx + 1 < cb and type byte is valid
 * (not 0x00, not 0xFF).
 * On success: *pIdx set to first byte after the segment; return 0.
 * On truncation / bad length: return -1 (stream unusable past here).
 */
static int
b110_skip_marker(const unsigned char *pBuf, size_t cb, size_t *pIdx)
{
	size_t i = *pIdx;
	unsigned uType;
	unsigned uSeg;
	size_t iType;
	size_t iNext;

	/* Skip any 0xFF fill bytes; type is the first non-0xFF after them. */
	while (i < cb && pBuf[i] == 0xFFu) {
		i++;
	}
	if (i >= cb) {
		return -1;
	}
	uType = (unsigned)pBuf[i];
	iType = i;

	if (uType == 0x00u) {
		/* Should not be called for stuffing; treat as error. */
		return -1;
	}

	if (b110_is_standalone(uType)) {
		*pIdx = iType + 1u;
		return 0;
	}

	/* Length field starts at iType + 1. */
	if (iType + 3u > cb) {
		return -1;
	}
	uSeg = b110_be16(pBuf + iType + 1u);
	if (uSeg < 2u) {
		return -1;
	}
	/* next = (type byte) + 1 + uSeg  == iType + 1 + uSeg */
	iNext = iType + 1u + (size_t)uSeg;
	if (iNext > cb) {
		return -1;
	}
	*pIdx = iNext;
	return 0;
}

/*
 * Locate the next JPEG marker at or after iStart.
 * Writes *pOff = offset of the 0xFF immediately preceding the type byte,
 * *pType = type byte, *pAfter = index of the type byte (for length reads).
 * Returns 0 on success, -1 if none remain / truncated mid-marker.
 *
 * Handles 0xFF fill, 0xFF00 entropy stuffing, and walks segment lengths
 * only when the caller will skip — this finder only reports the marker.
 */
static int
b110_next_marker(const unsigned char *pBuf, size_t cb, size_t iStart,
                 size_t *pOff, unsigned *pType, size_t *pTypeOff)
{
	size_t i = iStart;

	if (pBuf == NULL || pOff == NULL || pType == NULL || pTypeOff == NULL) {
		return -1;
	}

	while (i + 1u < cb) {
		size_t iFf;
		size_t j;
		unsigned uType;

		if (pBuf[i] != 0xFFu) {
			i++;
			continue;
		}

		/* Record first 0xFF of a possible fill run. */
		iFf = i;
		j = i + 1u;
		while (j < cb && pBuf[j] == 0xFFu) {
			j++;
		}
		if (j >= cb) {
			return -1;
		}
		uType = (unsigned)pBuf[j];
		if (uType == 0x00u) {
			/* Stuffed 0xFF00 inside entropy-coded data. */
			i = j + 1u;
			continue;
		}

		/*
		 * Marker type uType. Canonical 0xFF of the marker is the
		 * last 0xFF before the type byte (j - 1).
		 */
		*pOff = j - 1u;
		*pType = uType;
		*pTypeOff = j;
		(void)iFf;
		return 0;
	}
	return -1;
}

/* ---- public: gj_jpeg_is_jpeg ------------------------------------------- */

/*
 * True (1) if the buffer begins with SOI 0xFF D8.
 * False (0) for NULL, short, or non-JPEG magic.
 */
int
gj_jpeg_is_jpeg(const void *pBuf, size_t cb)
{
	const unsigned char *p;

	if (pBuf == NULL || cb < 2u) {
		return 0;
	}
	p = (const unsigned char *)pBuf;
	if (p[0] != 0xFFu || p[1] != (unsigned char)B110_SOI) {
		return 0;
	}
	return 1;
}

/* ---- public: gj_jpeg_find_marker --------------------------------------- */

/*
 * Find the first occurrence of marker type (marker & 0xFF).
 * On success: *pOff = byte offset of the marker's 0xFF, return 0.
 * On failure: return -1 (*pOff unchanged). pOff must be non-NULL.
 *
 * Scans the whole buffer (SOI/EOI/APP0/SOF/… and post-SOS with stuffing).
 */
int
gj_jpeg_find_marker(const void *pBuf, size_t cb, unsigned uMarker,
                    size_t *pOff)
{
	const unsigned char *p;
	unsigned uWant;
	size_t i;

	if (pBuf == NULL || pOff == NULL || cb < 2u) {
		return -1;
	}

	p = (const unsigned char *)pBuf;
	uWant = uMarker & 0xFFu;
	i = 0u;

	while (i + 1u < cb) {
		size_t uOff;
		size_t uTypeOff;
		unsigned uType;
		size_t iSeg;

		if (b110_next_marker(p, cb, i, &uOff, &uType, &uTypeOff) != 0) {
			return -1;
		}

		if (uType == uWant) {
			*pOff = uOff;
			return 0;
		}

		/*
		 * Advance past this marker so the next search does not
		 * re-hit the same 0xFF. Standalone: after type; others:
		 * after the length-prefixed segment.
		 */
		iSeg = uOff;
		if (b110_skip_marker(p, cb, &iSeg) != 0) {
			/*
			 * Truncated segment: cannot safely continue, but
			 * if the type already matched we would have
			 * returned above. Give up.
			 */
			return -1;
		}
		i = iSeg;
	}
	return -1;
}

/* ---- public: gj_jpeg_dimensions ---------------------------------------- */

/*
 * Locate first SOF0 (baseline) or SOF2 (progressive) frame header and
 * extract image height / width.
 *
 * Layout from type byte (0xC0 / 0xC2):
 *   type+1..+2  length (BE, includes these two bytes; >= 8)
 *   type+3      sample precision
 *   type+4..+5  height (BE)
 *   type+6..+7  width  (BE)
 *   type+8      number of components
 *
 * Returns 0 and writes *pW / *pH on success; -1 on any failure.
 * Requires SOI at start (minimal validity). Zero dimensions rejected.
 */
int
gj_jpeg_dimensions(const void *pBuf, size_t cb, unsigned *pW, unsigned *pH)
{
	const unsigned char *p;
	size_t i;

	if (pBuf == NULL || pW == NULL || pH == NULL) {
		return -1;
	}
	if (!gj_jpeg_is_jpeg(pBuf, cb)) {
		return -1;
	}

	p = (const unsigned char *)pBuf;
	/* Start after SOI (bytes 0..1). */
	i = 2u;

	while (i + 1u < cb) {
		size_t uOff;
		size_t uTypeOff;
		unsigned uType;
		unsigned uSeg;
		unsigned uH;
		unsigned uW;
		size_t iSeg;

		if (b110_next_marker(p, cb, i, &uOff, &uType, &uTypeOff) != 0) {
			return -1;
		}

		if (uType == B110_SOF0 || uType == B110_SOF2) {
			/*
			 * Need through width: type + len(2) + prec(1) + H(2)
			 * + W(2) → last byte at type+7 inclusive.
			 */
			if (uTypeOff + 8u > cb) {
				return -1;
			}
			uSeg = b110_be16(p + uTypeOff + 1u);
			if (uSeg < 8u) {
				return -1;
			}
			/* Ensure claimed segment fits. */
			if (uTypeOff + 1u + (size_t)uSeg > cb) {
				return -1;
			}
			uH = b110_be16(p + uTypeOff + 4u);
			uW = b110_be16(p + uTypeOff + 6u);
			if (uW == 0u || uH == 0u) {
				return -1;
			}
			*pW = uW;
			*pH = uH;
			return 0;
		}

		/* EOI without SOF: no dimensions. */
		if (uType == B110_EOI) {
			return -1;
		}

		iSeg = uOff;
		if (b110_skip_marker(p, cb, &iSeg) != 0) {
			return -1;
		}
		i = iSeg;
	}
	return -1;
}

/* ---- underscored aliases ----------------------------------------------- */

int __gj_jpeg_is_jpeg(const void *pBuf, size_t cb)
    __attribute__((alias("gj_jpeg_is_jpeg")));

int __gj_jpeg_find_marker(const void *pBuf, size_t cb, unsigned uMarker,
                          size_t *pOff)
    __attribute__((alias("gj_jpeg_find_marker")));

int __gj_jpeg_dimensions(const void *pBuf, size_t cb, unsigned *pW,
                         unsigned *pH)
    __attribute__((alias("gj_jpeg_dimensions")));
