/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch111: PNG signature + IHDR parse only.
 * Clean-room freestanding pure C — integer/pointer only (no IDAT inflate,
 * no zlib dependency). No third-party PNG source was copied.
 *
 * Surface:
 *   int gj_png_is_png(const void *buf, size_t len);
 *     1 if buf begins with the 8-byte PNG signature and len >= 8, else 0.
 *   int gj_png_ihdr(const void *buf, size_t len, unsigned *w, unsigned *h,
 *                   unsigned *bit_depth, unsigned *color_type);
 *     Parse the mandatory first IHDR chunk after the signature.
 *     Verifies chunk length (13), type "IHDR", CRC, and basic field rules.
 *     Returns 0 on success, -1 on error.
 *   uint32_t gj_png_crc32(const void *data, size_t len);
 *     One-shot IEEE / PNG CRC-32 (poly 0xEDB88320 reflected).
 *     Matches zlib crc32(0, data, len) / batch39 crc32_z(0, …).
 *     NULL data → 0.
 *   __gj_png_is_png / __gj_png_ihdr / __gj_png_crc32  (aliases)
 *   __libcgj_batch111_marker = "libcgj-batch111"
 *
 * Layout (ISO/IEC 15948 / W3C PNG):
 *   sig[8] | lenBE[4] type[4] data[len] crcBE[4] | …
 *   IHDR data (13): widthBE heightBE bit_depth color_type
 *                   compression filter interlace
 *   CRC covers type + data only (not length).
 *
 * Does NOT inflate IDAT or parse other chunks.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch111_marker[] = "libcgj-batch111";

/* PNG signature: 89 50 4E 47 0D 0A 1A 0A */
#define B111_SIG_LEN     8u
#define B111_IHDR_DATA   13u
/* sig + len + type + IHDR data + crc */
#define B111_MIN_IHDR    (B111_SIG_LEN + 4u + 4u + B111_IHDR_DATA + 4u)

/* Offsets from start of buffer */
#define B111_OFF_LEN     B111_SIG_LEN
#define B111_OFF_TYPE    (B111_SIG_LEN + 4u)
#define B111_OFF_DATA    (B111_SIG_LEN + 8u)
#define B111_OFF_CRC     (B111_SIG_LEN + 8u + B111_IHDR_DATA)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b111_load_be32(const unsigned char *p)
{
	return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
	       ((uint32_t)p[2] << 8) | (uint32_t)p[3];
}

static int
b111_sig_ok(const unsigned char *p, size_t cb)
{
	static const unsigned char aSig[B111_SIG_LEN] = {
	    0x89u, 0x50u, 0x4eu, 0x47u, 0x0du, 0x0au, 0x1au, 0x0au
	};
	size_t i;

	if (p == NULL || cb < B111_SIG_LEN) {
		return 0;
	}
	for (i = 0u; i < B111_SIG_LEN; i++) {
		if (p[i] != aSig[i]) {
			return 0;
		}
	}
	return 1;
}

/*
 * Allowed (bit_depth, color_type) pairs per PNG spec §11.2.2 / §7.1.
 * color_type: 0 grey, 2 RGB, 3 palette, 4 grey+A, 6 RGBA.
 */
static int
b111_depth_ok(unsigned uDepth, unsigned uCtype)
{
	switch (uCtype) {
	case 0u:
		return uDepth == 1u || uDepth == 2u || uDepth == 4u ||
		       uDepth == 8u || uDepth == 16u;
	case 2u:
	case 4u:
	case 6u:
		return uDepth == 8u || uDepth == 16u;
	case 3u:
		return uDepth == 1u || uDepth == 2u || uDepth == 4u ||
		       uDepth == 8u;
	default:
		return 0;
	}
}

/* ---- IEEE / PNG CRC-32 (local; freestanding -c needs no extern) -------- */

static uint32_t s_b111_crc_tab[256];
static int s_b111_tab_ready;

static void
b111_crc_init_tab(void)
{
	const uint32_t uPoly = 0xedb88320u;
	unsigned i;
	unsigned b;

	if (s_b111_tab_ready) {
		return;
	}
	for (i = 0u; i < 256u; i++) {
		uint32_t uC = (uint32_t)i;

		for (b = 0u; b < 8u; b++) {
			if ((uC & 1u) != 0u) {
				uC = uPoly ^ (uC >> 1);
			} else {
				uC >>= 1;
			}
		}
		s_b111_crc_tab[i] = uC;
	}
	s_b111_tab_ready = 1;
}

/*
 * One-shot PNG/zlib CRC-32. NULL data → 0. Matches batch39 crc32_z(0, …).
 */
uint32_t
gj_png_crc32(const void *pData, size_t cb)
{
	const unsigned char *p = (const unsigned char *)pData;
	uint32_t uCrc;
	size_t i;

	if (p == NULL) {
		return 0u;
	}
	b111_crc_init_tab();
	uCrc = 0xffffffffu;
	for (i = 0u; i < cb; i++) {
		uCrc = s_b111_crc_tab[(uCrc ^ p[i]) & 0xffu] ^ (uCrc >> 8);
	}
	return uCrc ^ 0xffffffffu;
}

/* ---- public: signature ------------------------------------------------- */

/*
 * Return 1 if the buffer starts with the PNG file signature, else 0.
 * Does not require a full IHDR; only the 8 signature bytes.
 */
int
gj_png_is_png(const void *pBuf, size_t cb)
{
	return b111_sig_ok((const unsigned char *)pBuf, cb);
}

/* ---- public: IHDR ------------------------------------------------------ */

/*
 * Parse width / height / bit_depth / color_type from the first chunk.
 * Requires signature + complete IHDR (length 13, type IHDR, good CRC).
 * Rejects zero dimensions, non-zero compression/filter method, bad
 * interlace (>1), and illegal bit_depth/color_type pairs.
 * Out pointers must all be non-NULL. Returns 0 / -1.
 */
int
gj_png_ihdr(const void *pBuf, size_t cb, unsigned *pW, unsigned *pH,
            unsigned *pBitDepth, unsigned *pColorType)
{
	const unsigned char *p;
	uint32_t uLen;
	uint32_t uStored;
	uint32_t uCalc;
	unsigned uW;
	unsigned uH;
	unsigned uDepth;
	unsigned uCtype;
	unsigned uComp;
	unsigned uFilt;
	unsigned uIntl;

	if (pBuf == NULL || pW == NULL || pH == NULL || pBitDepth == NULL ||
	    pColorType == NULL) {
		return -1;
	}
	if (cb < B111_MIN_IHDR) {
		return -1;
	}
	p = (const unsigned char *)pBuf;
	if (!b111_sig_ok(p, cb)) {
		return -1;
	}

	uLen = b111_load_be32(p + B111_OFF_LEN);
	if (uLen != B111_IHDR_DATA) {
		return -1;
	}
	/* type must be 'I' 'H' 'D' 'R' */
	if (p[B111_OFF_TYPE + 0u] != (unsigned char)'I' ||
	    p[B111_OFF_TYPE + 1u] != (unsigned char)'H' ||
	    p[B111_OFF_TYPE + 2u] != (unsigned char)'D' ||
	    p[B111_OFF_TYPE + 3u] != (unsigned char)'R') {
		return -1;
	}

	/* CRC over type (4) + data (13) = 17 bytes starting at OFF_TYPE */
	uCalc = gj_png_crc32(p + B111_OFF_TYPE, 4u + B111_IHDR_DATA);
	uStored = b111_load_be32(p + B111_OFF_CRC);
	if (uCalc != uStored) {
		return -1;
	}

	uW = (unsigned)b111_load_be32(p + B111_OFF_DATA + 0u);
	uH = (unsigned)b111_load_be32(p + B111_OFF_DATA + 4u);
	uDepth = (unsigned)p[B111_OFF_DATA + 8u];
	uCtype = (unsigned)p[B111_OFF_DATA + 9u];
	uComp = (unsigned)p[B111_OFF_DATA + 10u];
	uFilt = (unsigned)p[B111_OFF_DATA + 11u];
	uIntl = (unsigned)p[B111_OFF_DATA + 12u];

	if (uW == 0u || uH == 0u) {
		return -1;
	}
	if (uComp != 0u || uFilt != 0u) {
		return -1;
	}
	if (uIntl > 1u) {
		return -1;
	}
	if (!b111_depth_ok(uDepth, uCtype)) {
		return -1;
	}

	*pW = uW;
	*pH = uH;
	*pBitDepth = uDepth;
	*pColorType = uCtype;
	return 0;
}

int __gj_png_is_png(const void *p, size_t n)
    __attribute__((alias("gj_png_is_png")));
int __gj_png_ihdr(const void *p, size_t n, unsigned *pW, unsigned *pH,
                  unsigned *pBd, unsigned *pCt)
    __attribute__((alias("gj_png_ihdr")));
uint32_t __gj_png_crc32(const void *p, size_t n)
    __attribute__((alias("gj_png_crc32")));
