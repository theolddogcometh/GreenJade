/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch153: BGP OPEN message header parse (RFC 4271).
 * Clean-room freestanding pure C from the public BGP OPEN layout
 * (RFC 4271 §4.2) — fixed fields after the common 19-octet message
 * header, before optional parameters:
 *   Version             1 octet
 *   My Autonomous System 2 octets, network (big-endian)
 *   Hold Time            2 octets, network (big-endian)
 *   BGP Identifier       4 octets (IPv4-shaped, host order not applied)
 * Builds under userspace -msse2. No third-party BGP source was copied.
 *
 * Surface (unique symbols):
 *   int gj_bgp_parse_open(const unsigned char *in, size_t len,
 *                         unsigned *version, unsigned *my_as,
 *                         unsigned *hold, unsigned char bgp_id[4]);
 *     Parse the OPEN fixed fields from in[0..len). in may be either:
 *       (1) the OPEN body starting at Version (offset 0 = Version), or
 *       (2) a full BGP message starting at the 16-octet Marker when the
 *           common header has Type == OPEN (1); body is then at offset 19.
 *     On success fills *version, *my_as, *hold and bgp_id[0..3].
 *     Returns 0 on success, -1 on short/NULL input or non-OPEN type when
 *     a full message header is present.
 *   __gj_bgp_parse_open  (alias)
 *   __libcgj_batch153_marker = "libcgj-batch153"
 *
 * Note: does not enforce Version == 4, Hold Time policy, or AS ranges —
 * callers apply session policy. Optional Parameters Length / TLVs are
 * not consumed (header-only).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch153_marker[] = "libcgj-batch153";

/* BGP common message header size (Marker + Length + Type). */
#define B153_COMMON_HDR 19u
/* OPEN fixed fields: Version + MyAS + Hold + BGP Identifier. */
#define B153_OPEN_FIXED 9u
/* Minimum full OPEN message (common header + fixed body, no opts). */
#define B153_MIN_FULL   (B153_COMMON_HDR + B153_OPEN_FIXED)
/* BGP message type OPEN (RFC 4271 §4.1). */
#define B153_TYPE_OPEN  1u

/* ---- big-endian load --------------------------------------------------- */

static unsigned
b153_rd16be(const unsigned char *p)
{
	return ((unsigned)p[0] << 8) | (unsigned)p[1];
}

/*
 * True when p[0..15] is the all-ones BGP Marker used for OPEN / KEEPALIVE
 * / NOTIFICATION / unauthenticated UPDATE (RFC 4271 §4.1).
 */
static int
b153_marker_all_ones(const unsigned char *p)
{
	unsigned i;

	for (i = 0u; i < 16u; i++) {
		if (p[i] != 0xffu) {
			return 0;
		}
	}
	return 1;
}

/* ---- gj_bgp_parse_open ------------------------------------------------- */

/*
 * Decode OPEN fixed header fields from a wire buffer.
 * All out pointers (including bgp_id) are required.
 */
int
gj_bgp_parse_open(const unsigned char *pIn, size_t cbLen, unsigned *puVersion,
    unsigned *puMyAs, unsigned *puHold, unsigned char aBgpId[4])
{
	const unsigned char *pBody;
	size_t cbBody;
	unsigned uVersion;
	unsigned uMyAs;
	unsigned uHold;
	unsigned i;

	if (pIn == NULL || puVersion == NULL || puMyAs == NULL ||
	    puHold == NULL || aBgpId == NULL) {
		return -1;
	}

	/*
	 * Full BGP message path: Marker (16×0xff) + Length + Type=OPEN.
	 * Body begins at offset 19. Length field is not strictly enforced
	 * against cbLen beyond the fixed OPEN fields (callers may pass a
	 * larger buffer holding optional parameters).
	 */
	if (cbLen >= B153_MIN_FULL && b153_marker_all_ones(pIn)) {
		unsigned uType = (unsigned)pIn[18];

		if (uType != B153_TYPE_OPEN) {
			return -1;
		}
		pBody = pIn + B153_COMMON_HDR;
		cbBody = cbLen - B153_COMMON_HDR;
	} else {
		/* OPEN body only: Version at offset 0. */
		pBody = pIn;
		cbBody = cbLen;
	}

	if (cbBody < B153_OPEN_FIXED) {
		return -1;
	}

	uVersion = (unsigned)pBody[0];
	uMyAs = b153_rd16be(pBody + 1);
	uHold = b153_rd16be(pBody + 3);

	for (i = 0u; i < 4u; i++) {
		aBgpId[i] = pBody[5u + i];
	}

	*puVersion = uVersion;
	*puMyAs = uMyAs;
	*puHold = uHold;
	return 0;
}

int __gj_bgp_parse_open(const unsigned char *in, size_t len,
    unsigned *version, unsigned *my_as, unsigned *hold,
    unsigned char bgp_id[4])
    __attribute__((alias("gj_bgp_parse_open")));
