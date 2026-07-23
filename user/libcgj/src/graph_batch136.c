/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch136: TLS record-layer header helpers.
 * Clean-room freestanding pure C from the public TLS record layout
 * (RFC 5246 §6.2 / RFC 8446 §5.1) — 5-byte fixed header:
 *   ContentType type;          // 1 octet
 *   ProtocolVersion version;   // 2 octets, network (big-endian)
 *   uint16 length;             // 2 octets, network (big-endian)
 * Builds under userspace -msse2. No third-party TLS source was copied.
 *
 * Surface (unique symbols):
 *   int gj_tls_record_header_parse(const unsigned char *in, size_t len,
 *                                  unsigned *type, unsigned *version,
 *                                  unsigned *rec_len);
 *     Parse a 5-byte TLS record header. type = ContentType, version =
 *     (major << 8) | minor, rec_len = fragment length field. Returns 0
 *     on success, -1 on short/NULL input.
 *   int gj_tls_record_header_build(unsigned char out[5], unsigned type,
 *                                  unsigned version, unsigned rec_len);
 *     Write a 5-byte TLS record header. type must fit in 8 bits;
 *     version and rec_len must fit in 16 bits. Returns 0 ok, -1 error.
 *   __gj_tls_record_header_parse / __gj_tls_record_header_build (aliases)
 *   __libcgj_batch136_marker = "libcgj-batch136"
 *
 * Note: these helpers only encode/decode the header wire fields. They
 * do not enforce ContentType ranges, version suites, or the TLS
 * maximum record size (2^14 + overhead) — callers apply policy.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch136_marker[] = "libcgj-batch136";

/* Fixed TLS record header size (type + version + length). */
#define B136_HDR_LEN 5u

/* ---- gj_tls_record_header_parse ---------------------------------------- */

/*
 * Decode a TLSPlaintext / TLSCiphertext / TLSInnerPlaintext-compatible
 * 5-byte header from in[0..len). All out pointers are required.
 */
int
gj_tls_record_header_parse(const unsigned char *pIn, size_t cbLen,
                           unsigned *puType, unsigned *puVersion,
                           unsigned *puRecLen)
{
	unsigned uType;
	unsigned uVersion;
	unsigned uRecLen;

	if (pIn == NULL || puType == NULL || puVersion == NULL ||
	    puRecLen == NULL) {
		return -1;
	}
	if (cbLen < B136_HDR_LEN) {
		return -1;
	}

	uType = (unsigned)pIn[0];
	uVersion = ((unsigned)pIn[1] << 8) | (unsigned)pIn[2];
	uRecLen = ((unsigned)pIn[3] << 8) | (unsigned)pIn[4];

	*puType = uType;
	*puVersion = uVersion;
	*puRecLen = uRecLen;
	return 0;
}

int __gj_tls_record_header_parse(const unsigned char *in, size_t len,
                                 unsigned *type, unsigned *version,
                                 unsigned *rec_len)
    __attribute__((alias("gj_tls_record_header_parse")));

/* ---- gj_tls_record_header_build ---------------------------------------- */

/*
 * Encode type / version / rec_len into a 5-byte TLS record header.
 * Rejects values that do not fit the on-wire field widths.
 */
int
gj_tls_record_header_build(unsigned char aOut[5], unsigned uType,
                           unsigned uVersion, unsigned uRecLen)
{
	if (aOut == NULL) {
		return -1;
	}
	/* type: 1 octet; version / length: uint16 network order. */
	if (uType > 0xffu || uVersion > 0xffffu || uRecLen > 0xffffu) {
		return -1;
	}

	aOut[0] = (unsigned char)uType;
	aOut[1] = (unsigned char)((uVersion >> 8) & 0xffu);
	aOut[2] = (unsigned char)(uVersion & 0xffu);
	aOut[3] = (unsigned char)((uRecLen >> 8) & 0xffu);
	aOut[4] = (unsigned char)(uRecLen & 0xffu);
	return 0;
}

int __gj_tls_record_header_build(unsigned char out[5], unsigned type,
                                 unsigned version, unsigned rec_len)
    __attribute__((alias("gj_tls_record_header_build")));
