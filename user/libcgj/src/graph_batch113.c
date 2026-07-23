/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch113: DNS message helpers (RFC 1035 wire).
 * Clean-room freestanding pure C — integer/pointer only (no network I/O,
 * no libc string). No third-party resolver source was copied.
 * Userspace may be built with -msse2; this TU does not require SSE.
 *
 * Surface:
 *   int gj_dns_pack_query(const char *name, unsigned qtype,
 *                         unsigned char *out, size_t outcap,
 *                         size_t *outlen);
 *     Build a standard recursive query (RD=1, QDCOUNT=1, class IN=1)
 *     for presentation-form name + qtype. No name compression.
 *   int gj_dns_skip_name(const unsigned char *msg, size_t len,
 *                        size_t *off);
 *     Advance *off past one wire domain name (labels or compression
 *     pointer). Bounds-checked against msg[0..len).
 *   int gj_dns_parse_header(const unsigned char *msg, size_t len,
 *                           unsigned *id, unsigned *flags,
 *                           unsigned *qdcount);
 *     Parse the 12-byte DNS header (ID, FLAGS, QDCOUNT only).
 *   __gj_dns_pack_query / __gj_dns_skip_name / __gj_dns_parse_header
 *   __libcgj_batch113_marker = "libcgj-batch113"
 *
 * Semantics: 0 success, -1 error (errno set when available path used).
 * Wire multi-byte fields are big-endian per RFC 1035 §2.3 / §4.1.1.
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch113_marker[] = "libcgj-batch113";

/* RFC 1035 limits */
#define B113_HDR_LEN     12u
#define B113_MAX_LABEL   63u
#define B113_MAX_NAME    255u
#define B113_CLASS_IN    1u
#define B113_FLAG_RD     0x0100u
#define B113_PTR_MASK    0xc0u
#define B113_PTR_OFF_MASK 0x3fffu
#define B113_MAX_PTR_HOPS 32u

/* Monotonic query id (freestanding; wraps naturally). */
static unsigned s_b113_qid = 0x1130u;

/* ---- freestanding helpers ---------------------------------------------- */

static void
b113_put_be16(unsigned char *pOut, unsigned uVal)
{
	pOut[0] = (unsigned char)((uVal >> 8) & 0xffu);
	pOut[1] = (unsigned char)(uVal & 0xffu);
}

static unsigned
b113_get_be16(const unsigned char *pIn)
{
	return ((unsigned)pIn[0] << 8) | (unsigned)pIn[1];
}

/*
 * Encode presentation name (dot-separated labels) into wire form at pOut.
 * Empty string or "." → root label (single 0). No compression.
 * Returns encoded length including root 0, or (size_t)-1 on error.
 * Does not write if cbCap is too small (still validates name when possible).
 */
static size_t
b113_encode_name(const char *szName, unsigned char *pOut, size_t cbCap)
{
	const char *p;
	size_t cb = 0u;
	size_t cbLabel;
	size_t i;

	if (szName == NULL) {
		return (size_t)-1;
	}

	/* Root */
	if (szName[0] == '\0' ||
	    (szName[0] == '.' && szName[1] == '\0')) {
		if (cbCap < 1u) {
			return (size_t)-1;
		}
		if (pOut != NULL) {
			pOut[0] = 0u;
		}
		return 1u;
	}

	p = szName;
	while (*p != '\0') {
		/* measure label */
		cbLabel = 0u;
		while (p[cbLabel] != '\0' && p[cbLabel] != '.') {
			cbLabel++;
		}
		if (cbLabel == 0u) {
			/* empty label (leading/double/trailing junk) */
			return (size_t)-1;
		}
		if (cbLabel > B113_MAX_LABEL) {
			return (size_t)-1;
		}
		/* wire total with this label + root 0 must be <= 255 */
		if (cb + 1u + cbLabel + 1u > B113_MAX_NAME) {
			return (size_t)-1;
		}
		if (cb + 1u + cbLabel >= cbCap) {
			return (size_t)-1;
		}
		if (pOut != NULL) {
			pOut[cb] = (unsigned char)cbLabel;
			for (i = 0u; i < cbLabel; i++) {
				pOut[cb + 1u + i] = (unsigned char)p[i];
			}
		}
		cb += 1u + cbLabel;
		p += cbLabel;
		if (*p == '.') {
			p++;
			if (*p == '\0') {
				/* trailing dot: FQDN, stop before empty label */
				break;
			}
		}
	}

	/* root terminator; total wire name length <= 255 */
	if (cb + 1u > B113_MAX_NAME) {
		return (size_t)-1;
	}
	if (cb + 1u > cbCap) {
		return (size_t)-1;
	}
	if (pOut != NULL) {
		pOut[cb] = 0u;
	}
	return cb + 1u;
}

/* ---- public: gj_dns_pack_query ----------------------------------------- */

/*
 * Pack a minimal DNS query message:
 *   [header 12][qname][qtype 2][qclass 2=IN]
 * ID is a local monotonic counter. FLAGS = RD only. QDCOUNT=1.
 * *pOutLen receives total message length on success.
 */
int
gj_dns_pack_query(const char *szName, unsigned uQtype, unsigned char *pOut,
                  size_t cbOutCap, size_t *pOutLen)
{
	size_t cbName;
	size_t cbNeed;
	unsigned uId;

	if (szName == NULL || pOut == NULL || pOutLen == NULL) {
		errno = EINVAL;
		return -1;
	}
	if (uQtype > 0xffffu) {
		errno = EINVAL;
		return -1;
	}

	/* Header + name + type + class */
	if (cbOutCap < B113_HDR_LEN + 1u + 4u) {
		errno = ENOSPC;
		return -1;
	}

	cbName = b113_encode_name(szName, pOut + B113_HDR_LEN,
	                          cbOutCap - B113_HDR_LEN - 4u);
	if (cbName == (size_t)-1) {
		/* Distinguish capacity vs bad name roughly */
		if (b113_encode_name(szName, NULL, (size_t)-1 >> 1) ==
		    (size_t)-1) {
			errno = EINVAL;
		} else {
			errno = ENOSPC;
		}
		return -1;
	}

	cbNeed = B113_HDR_LEN + cbName + 4u;
	if (cbNeed > cbOutCap) {
		errno = ENOSPC;
		return -1;
	}

	uId = (s_b113_qid++ & 0xffffu);
	if (uId == 0u) {
		/* Prefer non-zero id; skip 0 after wrap. */
		uId = (s_b113_qid++ & 0xffffu);
		if (uId == 0u) {
			uId = 1u;
			s_b113_qid = 2u;
		}
	}

	b113_put_be16(pOut + 0, uId);
	b113_put_be16(pOut + 2, B113_FLAG_RD);
	b113_put_be16(pOut + 4, 1u); /* QDCOUNT */
	b113_put_be16(pOut + 6, 0u); /* ANCOUNT */
	b113_put_be16(pOut + 8, 0u); /* NSCOUNT */
	b113_put_be16(pOut + 10, 0u); /* ARCOUNT */
	/* qname already written at pOut + 12 */
	b113_put_be16(pOut + B113_HDR_LEN + cbName, uQtype);
	b113_put_be16(pOut + B113_HDR_LEN + cbName + 2u, B113_CLASS_IN);

	*pOutLen = cbNeed;
	return 0;
}

/* ---- public: gj_dns_skip_name ------------------------------------------ */

/*
 * Skip one domain name starting at *pOff within msg[0..cbLen).
 * Handles:
 *   - sequence of labels ended by 0
 *   - compression pointer (0xC0xx) as the last element of the on-wire span
 * On success *pOff is advanced past the on-wire encoding of this name
 * (pointer consumes 2 bytes; labels consume their bytes including root 0).
 * Does not follow pointers for output — only validates hop chain stays
 * inside the message so a pointer is well-formed.
 */
int
gj_dns_skip_name(const unsigned char *pMsg, size_t cbLen, size_t *pOff)
{
	size_t uOff;
	size_t uFollow;
	unsigned uHops;
	unsigned uLab;
	int fSeenPtr;

	if (pMsg == NULL || pOff == NULL) {
		errno = EINVAL;
		return -1;
	}

	uOff = *pOff;
	if (uOff >= cbLen) {
		errno = EINVAL;
		return -1;
	}

	/*
	 * Walk the on-wire name at uOff. Compression ends the on-wire span.
	 * Separately validate pointer targets do not loop forever.
	 */
	fSeenPtr = 0;
	uFollow = uOff;
	uHops = 0u;

	for (;;) {
		if (uFollow >= cbLen) {
			errno = EINVAL;
			return -1;
		}
		uLab = (unsigned)pMsg[uFollow];

		if (uLab == 0u) {
			/* root label */
			if (!fSeenPtr) {
				if (uOff + 1u > cbLen) {
					errno = EINVAL;
					return -1;
				}
				*pOff = uOff + 1u;
			}
			return 0;
		}

		if ((uLab & B113_PTR_MASK) == B113_PTR_MASK) {
			/* compression pointer: 2 bytes on wire at first sight */
			unsigned uPtr;

			if (uFollow + 1u >= cbLen) {
				errno = EINVAL;
				return -1;
			}
			uPtr = (((unsigned)pMsg[uFollow] & 0x3fu) << 8) |
			       (unsigned)pMsg[uFollow + 1u];
			if (uPtr >= cbLen) {
				errno = EINVAL;
				return -1;
			}
			if (!fSeenPtr) {
				/* on-wire span ends after the 2-byte pointer */
				if (uOff + 2u > cbLen) {
					errno = EINVAL;
					return -1;
				}
				*pOff = uOff + 2u;
				fSeenPtr = 1;
			}
			/* follow to validate */
			uFollow = (size_t)uPtr;
			if (++uHops > B113_MAX_PTR_HOPS) {
				errno = EINVAL;
				return -1;
			}
			continue;
		}

		if ((uLab & B113_PTR_MASK) != 0u) {
			/* 0x40 / 0x80 reserved */
			errno = EINVAL;
			return -1;
		}

		/* ordinary label */
		if (uLab > B113_MAX_LABEL) {
			errno = EINVAL;
			return -1;
		}
		if (uFollow + 1u + (size_t)uLab > cbLen) {
			errno = EINVAL;
			return -1;
		}
		if (!fSeenPtr) {
			/* still counting on-wire bytes */
			uOff = uFollow + 1u + (size_t)uLab;
			if (uOff > cbLen) {
				errno = EINVAL;
				return -1;
			}
		}
		uFollow = uFollow + 1u + (size_t)uLab;
		if (++uHops > B113_MAX_NAME + B113_MAX_PTR_HOPS) {
			errno = EINVAL;
			return -1;
		}
	}
}

/* ---- public: gj_dns_parse_header --------------------------------------- */

/*
 * Read ID, FLAGS, and QDCOUNT from a DNS message header.
 * Requires at least 12 bytes. Other count fields are ignored.
 */
int
gj_dns_parse_header(const unsigned char *pMsg, size_t cbLen, unsigned *pId,
                    unsigned *pFlags, unsigned *pQdcount)
{
	if (pMsg == NULL || pId == NULL || pFlags == NULL ||
	    pQdcount == NULL) {
		errno = EINVAL;
		return -1;
	}
	if (cbLen < B113_HDR_LEN) {
		errno = EINVAL;
		return -1;
	}

	*pId = b113_get_be16(pMsg + 0);
	*pFlags = b113_get_be16(pMsg + 2);
	*pQdcount = b113_get_be16(pMsg + 4);
	return 0;
}

/* ---- underscored aliases ----------------------------------------------- */

int __gj_dns_pack_query(const char *szName, unsigned uQtype,
                        unsigned char *pOut, size_t cbOutCap, size_t *pOutLen)
    __attribute__((alias("gj_dns_pack_query")));
int __gj_dns_skip_name(const unsigned char *pMsg, size_t cbLen, size_t *pOff)
    __attribute__((alias("gj_dns_skip_name")));
int __gj_dns_parse_header(const unsigned char *pMsg, size_t cbLen,
                          unsigned *pId, unsigned *pFlags, unsigned *pQdcount)
    __attribute__((alias("gj_dns_parse_header")));
