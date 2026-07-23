/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch343: TLS record-layer 5-byte header parse.
 * Clean-room freestanding pure C from the public TLS record layout
 * (RFC 5246 §6.2 / RFC 8446 §5.1) — fixed 5-byte header:
 *   ContentType type;          // 1 octet
 *   ProtocolVersion version;   // 2 octets, network (big-endian)
 *   uint16 length;             // 2 octets, network (big-endian)
 *
 * Surface (unique symbols):
 *   int gj_tls_rec_parse(const uint8_t *hdr5, uint8_t *type,
 *                        uint16_t *ver, uint16_t *len);
 *     — Parse a 5-byte TLS record header at hdr5. Writes ContentType
 *       to *type, (major << 8) | minor to *ver, and fragment length
 *       field to *len. Returns 0 on success, -1 on NULL args.
 *   int __gj_tls_rec_parse  (alias)
 *   __libcgj_batch343_marker = "libcgj-batch343"
 *
 * Distinct from gj_tls_record_header_parse (batch136): this surface
 * takes a fixed 5-byte pointer and narrow uint8_t / uint16_t outs.
 * Does not enforce ContentType ranges, version suites, or the TLS
 * maximum record size — callers apply policy.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party TLS source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch343_marker[] = "libcgj-batch343";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tls_rec_parse — decode a fixed 5-byte TLS record header.
 *
 * hdr5:  pointer to type | major | minor | len_hi | len_lo
 * type:  out ContentType (1 octet)
 * ver:   out ProtocolVersion as host uint16 (network-order wire)
 * len:   out fragment length field as host uint16
 *
 * All out pointers required. On success returns 0; on NULL args -1
 * (no errno; freestanding).
 */
int
gj_tls_rec_parse(const uint8_t *pHdr5, uint8_t *pType, uint16_t *pVer,
                 uint16_t *pLen)
{
	uint8_t u8Type;
	uint16_t u16Ver;
	uint16_t u16Len;

	if (pHdr5 == NULL || pType == NULL || pVer == NULL || pLen == NULL) {
		return -1;
	}

	u8Type = pHdr5[0];
	u16Ver = (uint16_t)(((uint16_t)pHdr5[1] << 8) | (uint16_t)pHdr5[2]);
	u16Len = (uint16_t)(((uint16_t)pHdr5[3] << 8) | (uint16_t)pHdr5[4]);

	*pType = u8Type;
	*pVer = u16Ver;
	*pLen = u16Len;
	return 0;
}

int __gj_tls_rec_parse(const uint8_t *hdr5, uint8_t *type, uint16_t *ver,
                       uint16_t *len)
    __attribute__((alias("gj_tls_rec_parse")));
