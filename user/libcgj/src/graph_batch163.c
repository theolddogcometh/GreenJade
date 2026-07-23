/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch163: UUID version 7 layout helpers (RFC 9562).
 * Clean-room freestanding pure C — integer/pointer only (no SSE required;
 * builds under userspace -msse2). No third-party UUID source was copied.
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   uuid_type / uuid_variant / uuid_time → graph_batch38.c (libuuid)
 *   gj_uuid_parse / gj_uuid_unparse       → graph_batch116.c
 *   uuid_parse / uuid_generate*           → graph_batch36.c
 *
 * This TU adds only unique gj_* symbols (layout only — no RNG):
 *   void gj_uuid_v7_set_timestamp(unsigned char uuid[16], uint64_t unix_ms);
 *     — write 48-bit big-endian unix epoch milliseconds into octets 0..5,
 *       force version nibble = 7 on octet 6, force RFC 4122/9562 variant
 *       bits (10) on octet 8. Caller supplies rand_a / rand_b (or any fill)
 *       in the remaining bit fields; this helper does not generate entropy.
 *   int  gj_uuid_version(const unsigned char uuid[16]);
 *     — return version field (high nibble of octet 6), or -1 if NULL.
 *   int  gj_uuid_variant(const unsigned char uuid[16]);
 *     — return variant class of octet 8 (same codes as libuuid uuid_variant):
 *         0 = NCS, 1 = DCE / RFC 4122, 2 = Microsoft, 3 = other; -1 if NULL.
 *   __gj_uuid_v7_set_timestamp / __gj_uuid_version / __gj_uuid_variant
 *   __libcgj_batch163_marker = "libcgj-batch163"
 *
 * RFC 9562 UUID v7 on-wire layout (network / sequential octet order):
 *   octets 0..5  : unix_ts_ms (48-bit big-endian)
 *   octet  6     : ver (4) | rand_a high (4)
 *   octet  7     : rand_a low (8)
 *   octet  8     : var (2) | rand_b high (6)
 *   octets 9..15 : rand_b remainder
 *
 * Only the lower 48 bits of unix_ms are stored (overflow wraps in the
 * timestamp field). No clock-sequence / counter state is maintained here.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch163_marker[] = "libcgj-batch163";

/* ---- gj_uuid_v7_set_timestamp ------------------------------------------ */

/*
 * Stamp a 48-bit millisecond timestamp and fix version/variant layout bits.
 * Leaves rand_a low nibble (uuid[6] & 0x0f), uuid[7], and rand_b lower bits
 * (uuid[8] & 0x3f plus uuid[9..15]) untouched for the caller to fill.
 */
void
gj_uuid_v7_set_timestamp(unsigned char uuid[16], uint64_t unix_ms)
{
	uint64_t uTs;

	if (uuid == NULL) {
		return;
	}

	/* 48-bit big-endian unix milliseconds into octets 0..5 */
	uTs = unix_ms & 0x0000ffffffffffffull;
	uuid[0] = (unsigned char)((uTs >> 40) & 0xffu);
	uuid[1] = (unsigned char)((uTs >> 32) & 0xffu);
	uuid[2] = (unsigned char)((uTs >> 24) & 0xffu);
	uuid[3] = (unsigned char)((uTs >> 16) & 0xffu);
	uuid[4] = (unsigned char)((uTs >> 8) & 0xffu);
	uuid[5] = (unsigned char)(uTs & 0xffu);

	/* version = 7 in high nibble of octet 6; keep rand_a high 4 bits */
	uuid[6] = (unsigned char)((uuid[6] & 0x0fu) | 0x70u);

	/* RFC 4122 / 9562 variant 10xxxxxx in octet 8; keep rand_b high 6 */
	uuid[8] = (unsigned char)((uuid[8] & 0x3fu) | 0x80u);
}

void __gj_uuid_v7_set_timestamp(unsigned char uuid[16], uint64_t unix_ms)
    __attribute__((alias("gj_uuid_v7_set_timestamp")));

/* ---- gj_uuid_version --------------------------------------------------- */

/*
 * Extract the 4-bit version field (high nibble of octet 6).
 * Returns -1 on NULL; otherwise 0..15 (v7 yields 7 after set_timestamp).
 */
int
gj_uuid_version(const unsigned char uuid[16])
{
	if (uuid == NULL) {
		return -1;
	}
	return (int)((uuid[6] >> 4) & 0x0fu);
}

int __gj_uuid_version(const unsigned char uuid[16])
    __attribute__((alias("gj_uuid_version")));

/* ---- gj_uuid_variant --------------------------------------------------- */

/*
 * Classify the variant bits of octet 8 (libuuid-compatible return codes):
 *   0  NCS compatibility      (0xxx xxxx)
 *   1  DCE / RFC 4122 / 9562  (10xx xxxx)
 *   2  Microsoft              (110x xxxx)
 *   3  reserved / other       (111x xxxx)
 */
int
gj_uuid_variant(const unsigned char uuid[16])
{
	unsigned char c;

	if (uuid == NULL) {
		return -1;
	}
	c = uuid[8];
	if ((c & 0x80u) == 0u) {
		return 0; /* NCS */
	}
	if ((c & 0xc0u) == 0x80u) {
		return 1; /* DCE / RFC 4122 */
	}
	if ((c & 0xe0u) == 0xc0u) {
		return 2; /* Microsoft */
	}
	return 3; /* other */
}

int __gj_uuid_variant(const unsigned char uuid[16])
    __attribute__((alias("gj_uuid_variant")));
