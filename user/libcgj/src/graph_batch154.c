/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch154: NTP packet header parse (RFC 5905).
 * Clean-room freestanding pure C — integer/pointer only (builds under
 * userspace -msse2). No third-party NTP source was copied.
 *
 * Surface (unique symbols):
 *   int gj_ntp_parse(const unsigned char *pkt, size_t len,
 *                    unsigned *li, unsigned *vn, unsigned *mode,
 *                    uint32_t *root_delay, uint32_t *xmt_sec);
 *     Parse a standard 48-byte NTP header (RFC 5905 §7.3). On success
 *     fills LI / VN / Mode from the first octet, Root Delay (big-endian
 *     NTP Short at offset 4), and the seconds half of the Transmit
 *     Timestamp (big-endian at offset 40). Returns 0 on success, -1 on
 *     NULL args or truncated packet (len < 48).
 *   __gj_ntp_parse  (alias)
 *   __libcgj_batch154_marker = "libcgj-batch154"
 *
 * RFC 5905 header layout (network byte order):
 *   offset  0 : LI (2) | VN (3) | Mode (3)
 *   offset  1 : Stratum
 *   offset  2 : Poll
 *   offset  3 : Precision
 *   offset  4 : Root Delay      (32-bit NTP Short)
 *   offset  8 : Root Dispersion (32-bit NTP Short)
 *   offset 12 : Reference ID
 *   offset 16 : Reference Timestamp (64-bit NTP Timestamp)
 *   offset 24 : Origin Timestamp
 *   offset 32 : Receive Timestamp
 *   offset 40 : Transmit Timestamp  (seconds @ 40, fraction @ 44)
 *
 * Minimum unauthenticated header without extension fields: 48 octets.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch154_marker[] = "libcgj-batch154";

/* Standard NTP header size (no MAC / extension fields). */
#define B154_HDR_MIN  48u

/* Field offsets within the header. */
#define B154_OFF_ROOT_DELAY  4u
#define B154_OFF_XMT_SEC     40u

/* ---- freestanding helpers ---------------------------------------------- */

/* Load a 32-bit big-endian word from an unaligned octet pointer. */
static uint32_t
b154_load_be32(const unsigned char *p)
{
	return ((uint32_t)p[0] << 24) |
	       ((uint32_t)p[1] << 16) |
	       ((uint32_t)p[2] << 8) |
	       (uint32_t)p[3];
}

/* ---- public: NTP header parse ------------------------------------------ */

/*
 * gj_ntp_parse — extract LI, VN, Mode, Root Delay, and Transmit seconds
 * from a wire-format NTP packet (RFC 5905 §7.3).
 *
 * First octet bit packing (MSB first as on the wire):
 *   bits 7..6 = LI, bits 5..3 = VN, bits 2..0 = Mode
 *
 * root_delay and xmt_sec are returned in host-endian uint32_t (wire
 * values are big-endian; no fixed-point scaling is applied).
 */
int
gj_ntp_parse(const unsigned char *pkt, size_t len, unsigned *li,
             unsigned *vn, unsigned *mode, uint32_t *root_delay,
             uint32_t *xmt_sec)
{
	unsigned char u0;

	if (pkt == NULL || li == NULL || vn == NULL || mode == NULL ||
	    root_delay == NULL || xmt_sec == NULL) {
		return -1;
	}
	if (len < B154_HDR_MIN) {
		return -1;
	}

	u0 = pkt[0];
	*li = (unsigned)((u0 >> 6) & 0x3u);
	*vn = (unsigned)((u0 >> 3) & 0x7u);
	*mode = (unsigned)(u0 & 0x7u);

	*root_delay = b154_load_be32(pkt + B154_OFF_ROOT_DELAY);
	*xmt_sec = b154_load_be32(pkt + B154_OFF_XMT_SEC);

	return 0;
}

int __gj_ntp_parse(const unsigned char *pkt, size_t len, unsigned *li,
                   unsigned *vn, unsigned *mode, uint32_t *root_delay,
                   uint32_t *xmt_sec)
    __attribute__((alias("gj_ntp_parse")));
