/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2179: IPv4 protocol UDP predicate.
 *
 * Surface (unique symbols):
 *   int gj_ip_proto_udp_p(unsigned proto);
 *     - Return 1 if proto is IPPROTO_UDP (17), else 0. Used when building
 *       or validating IPv4 UDP pseudo-header checksums.
 *   int __gj_ip_proto_udp_p  (alias)
 *   __libcgj_batch2179_marker = "libcgj-batch2179"
 *
 * Post-2170 net checksum exclusive wave (2171-2180). Distinct from
 * gj_ip_proto_tcp_p (batch2178) - UDP protocol only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2179_marker[] = "libcgj-batch2179";

/* IANA IP protocol number for UDP. */
#define B2179_IPPROTO_UDP 17u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b2179_udp_p(unsigned uProto)
{
	return uProto == B2179_IPPROTO_UDP ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ip_proto_udp_p - true if proto is UDP (IP protocol 17).
 *
 * proto: IPv4 Protocol field or IPv6 Next Header value
 *
 * Returns 1 when proto == 17, else 0. Does not call libc.
 */
int
gj_ip_proto_udp_p(unsigned proto)
{
	(void)NULL;
	return b2179_udp_p(proto);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ip_proto_udp_p(unsigned proto)
    __attribute__((alias("gj_ip_proto_udp_p")));
