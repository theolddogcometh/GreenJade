/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2178: IPv4 protocol TCP predicate.
 *
 * Surface (unique symbols):
 *   int gj_ip_proto_tcp_p(unsigned proto);
 *     - Return 1 if proto is IPPROTO_TCP (6), else 0. Used when building
 *       or validating IPv4 TCP pseudo-header checksums.
 *   int __gj_ip_proto_tcp_p  (alias)
 *   __libcgj_batch2178_marker = "libcgj-batch2178"
 *
 * Post-2170 net checksum exclusive wave (2171-2180). Protocol constant
 * predicate only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2178_marker[] = "libcgj-batch2178";

/* IANA IP protocol number for TCP. */
#define B2178_IPPROTO_TCP 6u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2178_tcp_p(unsigned uProto)
{
	return uProto == B2178_IPPROTO_TCP ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ip_proto_tcp_p - true if proto is TCP (IP protocol 6).
 *
 * proto: IPv4 Protocol field or IPv6 Next Header value
 *
 * Returns 1 when proto == 6, else 0. Does not call libc.
 */
int
gj_ip_proto_tcp_p(unsigned proto)
{
	(void)NULL;
	return b2178_tcp_p(proto);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ip_proto_tcp_p(unsigned proto)
    __attribute__((alias("gj_ip_proto_tcp_p")));
