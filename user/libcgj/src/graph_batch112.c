/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch112: WAV RIFF header parse helpers.
 * Clean-room freestanding pure C — integer/pointer only (no SSE required).
 * No third-party WAV / multimedia source was copied.
 *
 * Surface:
 *   int gj_wav_is_wav(const void *buf, size_t len);
 *     Returns 1 if buf begins with a RIFF/WAVE container header
 *     (magic "RIFF" at 0, "WAVE" at 8), else 0.
 *   int gj_wav_fmt(const void *buf, size_t len,
 *                  unsigned *channels, unsigned *sample_rate,
 *                  unsigned *bits);
 *     Locate the first "fmt " chunk and read nChannels, nSamplesPerSec,
 *     and wBitsPerSample (PCM layout). Returns 0 on success, -1 on error.
 *   int gj_wav_data_offset(const void *buf, size_t len,
 *                          size_t *off, size_t *data_len);
 *     Locate the first "data" chunk; *off is the payload start offset
 *     within buf, *data_len is the chunk size field. Returns 0 / -1.
 *   __gj_wav_is_wav / __gj_wav_fmt / __gj_wav_data_offset  (aliases)
 *   __libcgj_batch112_marker = "libcgj-batch112"
 *
 * RIFF/WAVE layout (little-endian):
 *   0..3   "RIFF"
 *   4..7   file size - 8 (ckSize of outer RIFF)
 *   8..11  "WAVE"
 *   12..   sub-chunks: id[4] size[4 LE] payload[size] [pad to even]
 *
 *   "fmt " payload (PCM, size >= 16):
 *     0  u16  wFormatTag
 *     2  u16  nChannels
 *     4  u32  nSamplesPerSec
 *     8  u32  nAvgBytesPerSec
 *    12  u16  nBlockAlign
 *    14  u16  wBitsPerSample
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch112_marker[] = "libcgj-batch112";

/* Minimum outer header: RIFF + size + WAVE */
#define B112_RIFF_HDR   12u
/* Sub-chunk header: id + size */
#define B112_CK_HDR     8u
/* PCM fmt payload minimum (through wBitsPerSample) */
#define B112_FMT_MIN    16u

/* ---- freestanding LE readers (unaligned-safe) -------------------------- */

static uint16_t
b112_le16(const unsigned char *p)
{
	return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

static uint32_t
b112_le32(const unsigned char *p)
{
	return (uint32_t)p[0] |
	       ((uint32_t)p[1] << 8) |
	       ((uint32_t)p[2] << 16) |
	       ((uint32_t)p[3] << 24);
}

static int
b112_id_eq(const unsigned char *p, char c0, char c1, char c2, char c3)
{
	return p[0] == (unsigned char)c0 && p[1] == (unsigned char)c1 &&
	       p[2] == (unsigned char)c2 && p[3] == (unsigned char)c3;
}

/* True if buffer has a RIFF/WAVE container header. */
static int
b112_is_wave(const unsigned char *p, size_t cb)
{
	if (p == NULL || cb < B112_RIFF_HDR) {
		return 0;
	}
	if (!b112_id_eq(p, 'R', 'I', 'F', 'F')) {
		return 0;
	}
	if (!b112_id_eq(p + 8, 'W', 'A', 'V', 'E')) {
		return 0;
	}
	return 1;
}

/*
 * Walk RIFF sub-chunks starting at offset 12.
 * On match of (c0..c3): *pOff = payload offset, *pCk = payload size.
 * Returns 1 if found (payload header fully present; payload may be
 * truncated relative to size), 0 if not found / malformed walk.
 */
static int
b112_find_chunk(const unsigned char *p, size_t cb,
                char c0, char c1, char c2, char c3,
                size_t *pOff, uint32_t *pCk)
{
	size_t uPos;

	if (p == NULL || pOff == NULL || pCk == NULL) {
		return 0;
	}
	if (!b112_is_wave(p, cb)) {
		return 0;
	}

	uPos = B112_RIFF_HDR;
	while (uPos + B112_CK_HDR <= cb) {
		const unsigned char *pCkHdr = p + uPos;
		uint32_t u32Size;
		size_t uPayload;
		size_t uNext;

		u32Size = b112_le32(pCkHdr + 4);
		uPayload = uPos + B112_CK_HDR;

		if (b112_id_eq(pCkHdr, c0, c1, c2, c3)) {
			*pOff = uPayload;
			*pCk = u32Size;
			return 1;
		}

		/*
		 * Advance past payload + optional pad byte (RIFF chunks are
		 * word-aligned). Overflow or truncated skip → stop walk.
		 */
		uNext = uPayload + (size_t)u32Size;
		if (uNext < uPayload) {
			return 0;
		}
		if ((u32Size & 1u) != 0u) {
			if (uNext + 1u < uNext) {
				return 0;
			}
			uNext += 1u;
		}
		if (uNext <= uPos || uNext > cb) {
			return 0;
		}
		uPos = uNext;
	}
	return 0;
}

/* ---- public: gj_wav_is_wav --------------------------------------------- */

/*
 * Return 1 if buf looks like a WAV container (RIFF…WAVE), else 0.
 * Does not require fmt/data chunks to be present in the buffer.
 */
int
gj_wav_is_wav(const void *pBuf, size_t cb)
{
	return b112_is_wave((const unsigned char *)pBuf, cb);
}

/* ---- public: gj_wav_fmt ------------------------------------------------ */

/*
 * Parse first "fmt " chunk: channels, sample_rate, bits-per-sample.
 * Requires at least 16 payload bytes (standard PCM fmt layout).
 * Out pointers must be non-NULL. Returns 0 on success, -1 on error.
 */
int
gj_wav_fmt(const void *pBuf, size_t cb, unsigned *pChannels,
           unsigned *pSampleRate, unsigned *pBits)
{
	const unsigned char *p;
	size_t uOff;
	uint32_t u32Ck;
	const unsigned char *pFmt;

	if (pChannels == NULL || pSampleRate == NULL || pBits == NULL) {
		return -1;
	}
	p = (const unsigned char *)pBuf;
	if (!b112_find_chunk(p, cb, 'f', 'm', 't', ' ', &uOff, &u32Ck)) {
		return -1;
	}
	/* Need full PCM fmt fields through wBitsPerSample. */
	if (u32Ck < (uint32_t)B112_FMT_MIN) {
		return -1;
	}
	if (uOff + B112_FMT_MIN > cb) {
		return -1;
	}

	pFmt = p + uOff;
	*pChannels = (unsigned)b112_le16(pFmt + 2);
	*pSampleRate = (unsigned)b112_le32(pFmt + 4);
	*pBits = (unsigned)b112_le16(pFmt + 14);
	return 0;
}

/* ---- public: gj_wav_data_offset ---------------------------------------- */

/*
 * Locate first "data" chunk.
 *   *pOff      = offset of PCM payload within buf
 *   *pDataLen  = chunk size field (declared payload length)
 * The declared length may exceed remaining buffer bytes; callers that
 * read samples must clamp to (len - *pOff). Returns 0 / -1.
 */
int
gj_wav_data_offset(const void *pBuf, size_t cb, size_t *pOff,
                   size_t *pDataLen)
{
	const unsigned char *p;
	size_t uOff;
	uint32_t u32Ck;

	if (pOff == NULL || pDataLen == NULL) {
		return -1;
	}
	p = (const unsigned char *)pBuf;
	if (!b112_find_chunk(p, cb, 'd', 'a', 't', 'a', &uOff, &u32Ck)) {
		return -1;
	}
	/* Payload start must at least be addressable (header was present). */
	if (uOff > cb) {
		return -1;
	}
	*pOff = uOff;
	*pDataLen = (size_t)u32Ck;
	return 0;
}

/* ---- underscored aliases ----------------------------------------------- */

int __gj_wav_is_wav(const void *pBuf, size_t cb)
    __attribute__((alias("gj_wav_is_wav")));
int __gj_wav_fmt(const void *pBuf, size_t cb, unsigned *pChannels,
                 unsigned *pSampleRate, unsigned *pBits)
    __attribute__((alias("gj_wav_fmt")));
int __gj_wav_data_offset(const void *pBuf, size_t cb, size_t *pOff,
                         size_t *pDataLen)
    __attribute__((alias("gj_wav_data_offset")));
