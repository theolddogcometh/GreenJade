/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch464: CBOR header peek (RFC 8949 initial byte +
 * argument). Clean-room freestanding pure C — integer/pointer only.
 * No third-party CBOR source was copied.
 *
 * Surface (unique symbols):
 *   int gj_cbor_peek(const uint8_t *p, size_t n, unsigned *major,
 *                    uint64_t *arg, size_t *hdr_len);
 *     — Parse CBOR initial byte + argument (0–8 follow bytes).
 *       On success: *major = major type (0..7), *arg = argument value,
 *       *hdr_len = total header bytes (1 + follow), return 0.
 *       On error (NULL args, empty/truncated, reserved ai 28..30,
 *       indefinite ai 31): return -1; out params unchanged.
 *   int __gj_cbor_peek  (alias)
 *   __libcgj_batch464_marker = "libcgj-batch464"
 *
 * Argument encoding (RFC 8949 §3):
 *   Header byte = (major << 5) | ai
 *   ai 0..23  value in ai;              hdr_len = 1
 *   ai 24     1-byte big-endian follow; hdr_len = 2
 *   ai 25     2-byte big-endian follow; hdr_len = 3
 *   ai 26     4-byte big-endian follow; hdr_len = 5
 *   ai 27     8-byte big-endian follow; hdr_len = 9
 *   ai 28..30 reserved — reject
 *   ai 31     indefinite / break — reject
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch464_marker[] = "libcgj-batch464";

/* Additional-info thresholds (RFC 8949). */
#define B464_AI_1B  24u
#define B464_AI_2B  25u
#define B464_AI_4B  26u
#define B464_AI_8B  27u

/* ---- freestanding helpers ---------------------------------------------- */

static uint16_t
b464_get_be16(const uint8_t *pBuf)
{
	return (uint16_t)(((uint16_t)pBuf[0] << 8) | (uint16_t)pBuf[1]);
}

static uint32_t
b464_get_be32(const uint8_t *pBuf)
{
	return ((uint32_t)pBuf[0] << 24) | ((uint32_t)pBuf[1] << 16) |
	       ((uint32_t)pBuf[2] << 8) | (uint32_t)pBuf[3];
}

static uint64_t
b464_get_be64(const uint8_t *pBuf)
{
	return ((uint64_t)pBuf[0] << 56) | ((uint64_t)pBuf[1] << 48) |
	       ((uint64_t)pBuf[2] << 40) | ((uint64_t)pBuf[3] << 32) |
	       ((uint64_t)pBuf[4] << 24) | ((uint64_t)pBuf[5] << 16) |
	       ((uint64_t)pBuf[6] << 8) | (uint64_t)pBuf[7];
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cbor_peek — decode CBOR initial byte + argument without consuming
 * payload. Does not interpret major type; any major 0..7 is accepted if
 * the argument encoding is well-formed and definite.
 */
int
gj_cbor_peek(const uint8_t *p, size_t n, unsigned *major, uint64_t *arg,
	     size_t *hdr_len)
{
	uint8_t u8b0;
	unsigned uAi;
	unsigned uMajor;
	uint64_t u64arg;
	size_t cbHead;

	if (p == NULL || major == NULL || arg == NULL || hdr_len == NULL) {
		return -1;
	}
	if (n == 0u) {
		return -1;
	}

	u8b0 = p[0];
	uMajor = (unsigned)(u8b0 >> 5);
	uAi = (unsigned)(u8b0 & 0x1fu);

	if (uAi < 24u) {
		u64arg = (uint64_t)uAi;
		cbHead = 1u;
	} else if (uAi == B464_AI_1B) {
		if (n < 2u) {
			return -1;
		}
		u64arg = (uint64_t)p[1];
		cbHead = 2u;
	} else if (uAi == B464_AI_2B) {
		if (n < 3u) {
			return -1;
		}
		u64arg = (uint64_t)b464_get_be16(p + 1);
		cbHead = 3u;
	} else if (uAi == B464_AI_4B) {
		if (n < 5u) {
			return -1;
		}
		u64arg = (uint64_t)b464_get_be32(p + 1);
		cbHead = 5u;
	} else if (uAi == B464_AI_8B) {
		if (n < 9u) {
			return -1;
		}
		u64arg = b464_get_be64(p + 1);
		cbHead = 9u;
	} else {
		/* ai 28..30 reserved; ai 31 indefinite/break — not supported. */
		return -1;
	}

	*major = uMajor;
	*arg = u64arg;
	*hdr_len = cbHead;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_cbor_peek(const uint8_t *p, size_t n, unsigned *major, uint64_t *arg,
		   size_t *hdr_len)
	__attribute__((alias("gj_cbor_peek")));
