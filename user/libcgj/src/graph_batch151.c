/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch151: SOCKS5 greeting / CONNECT request helpers.
 * Clean-room freestanding pure C from the public SOCKS Protocol Version 5
 * layout (RFC 1928 §§3–4). Builds under userspace -msse2. No third-party
 * SOCKS / proxy source was copied.
 *
 * Surface (unique symbols):
 *   int gj_socks5_parse_greeting(const unsigned char *in, size_t len,
 *                                unsigned *nmethods);
 *     Parse a client greeting (VER | NMETHODS | METHODS). Requires the full
 *     METHODS list to be present (len >= 2 + NMETHODS). VER must be 0x05.
 *     On success sets *nmethods = NMETHODS and returns 0; -1 on NULL/short
 *     input or wrong version.
 *   int gj_socks5_build_connect(unsigned char *out, size_t cap,
 *                               const char *host, unsigned port, size_t *n);
 *     Build a CONNECT request (CMD=0x01) using ATYP=DOMAINNAME (0x03):
 *       VER=0x05, CMD=0x01, RSV=0x00, ATYP=0x03,
 *       LEN, HOST[LEN], PORT_BE16
 *     host is a non-empty C string of length 1..255 (no embedded NUL counted
 *     past the terminator). port must fit in 16 bits. On success writes
 *     the request into out and sets *n = wire length (7 + hostlen). Returns
 *     0 ok, -1 on NULL/empty host, hostlen>255, port>0xffff, or cap short.
 *   __gj_socks5_parse_greeting / __gj_socks5_build_connect  (aliases)
 *   __libcgj_batch151_marker = "libcgj-batch151"
 *
 * Note: these helpers only encode/decode the named wire fields. They do
 * not open sockets, select auth methods, or parse server replies.
 */

#include <stddef.h>

const char __libcgj_batch151_marker[] = "libcgj-batch151";

/* SOCKS5 version byte (RFC 1928). */
#define B151_VER 0x05u
/* CMD CONNECT. */
#define B151_CMD_CONNECT 0x01u
/* ATYP domain name. */
#define B151_ATYP_DOMAIN 0x03u
/* Fixed prefix before domain: VER+CMD+RSV+ATYP+LEN = 5 octets. */
#define B151_CONN_FIXED 5u
/* Port field size. */
#define B151_PORT_LEN 2u

/* ---- gj_socks5_parse_greeting ------------------------------------------ */

/*
 * Decode VER / NMETHODS from a client greeting. METHODS octets themselves
 * are not copied; caller may read in[2 .. 2+*nmethods).
 */
int
gj_socks5_parse_greeting(const unsigned char *pIn, size_t cbLen,
    unsigned *puNmethods)
{
	unsigned uN;

	if (pIn == NULL || puNmethods == NULL) {
		return -1;
	}
	/* Need at least VER + NMETHODS. */
	if (cbLen < 2u) {
		return -1;
	}
	if ((unsigned)pIn[0] != B151_VER) {
		return -1;
	}

	uN = (unsigned)pIn[1];
	/* Full METHODS list must be present (may be empty: NMETHODS == 0). */
	if (cbLen < 2u + (size_t)uN) {
		return -1;
	}

	*puNmethods = uN;
	return 0;
}

int __gj_socks5_parse_greeting(const unsigned char *in, size_t len,
    unsigned *nmethods)
    __attribute__((alias("gj_socks5_parse_greeting")));

/* ---- gj_socks5_build_connect ------------------------------------------- */

/*
 * Encode a domain-name CONNECT request. Port is written big-endian.
 * Total wire size: 5 + hostlen + 2.
 */
int
gj_socks5_build_connect(unsigned char *pOut, size_t cbCap, const char *szHost,
    unsigned uPort, size_t *pcbN)
{
	size_t cbHost;
	size_t cbNeed;
	size_t i;

	if (pOut == NULL || szHost == NULL || pcbN == NULL) {
		return -1;
	}
	if (uPort > 0xffffu) {
		return -1;
	}

	/* Measure host without libc strlen (freestanding). */
	cbHost = 0u;
	while (szHost[cbHost] != '\0') {
		cbHost++;
		if (cbHost > 255u) {
			return -1;
		}
	}
	if (cbHost == 0u || cbHost > 255u) {
		return -1;
	}

	cbNeed = B151_CONN_FIXED + cbHost + B151_PORT_LEN;
	if (cbCap < cbNeed) {
		return -1;
	}

	pOut[0] = (unsigned char)B151_VER;
	pOut[1] = (unsigned char)B151_CMD_CONNECT;
	pOut[2] = 0x00u; /* RSV */
	pOut[3] = (unsigned char)B151_ATYP_DOMAIN;
	pOut[4] = (unsigned char)cbHost;
	for (i = 0u; i < cbHost; i++) {
		pOut[5u + i] = (unsigned char)szHost[i];
	}
	/* DST.PORT network (big-endian) octet order. */
	pOut[5u + cbHost] = (unsigned char)((uPort >> 8) & 0xffu);
	pOut[6u + cbHost] = (unsigned char)(uPort & 0xffu);

	*pcbN = cbNeed;
	return 0;
}

int __gj_socks5_build_connect(unsigned char *out, size_t cap, const char *host,
    unsigned port, size_t *n)
    __attribute__((alias("gj_socks5_build_connect")));
