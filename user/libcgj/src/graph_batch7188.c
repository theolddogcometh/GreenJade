/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7188: socket is-AF_INET domain predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sock_is_af_inet_u_7188(uint32_t domain);
 *     - Return 1 if domain is AF_INET (2), else 0.
 *   uint32_t __gj_sock_is_af_inet_u_7188  (alias)
 *   __libcgj_batch7188_marker = "libcgj-batch7188"
 *
 * Exclusive continuum CREATE-ONLY (7181-7190: socket domain/type stubs —
 * af_unix_id, af_inet_id, af_inet6_id, af_packet_id, sock_stream_id,
 * sock_dgram_id, sock_raw_id, is_af_inet, domain_type_ok, batch_id_7190).
 * Unique gj_sock_is_af_inet_u_7188 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7188_marker[] = "libcgj-batch7188";

/* AF_INET: IPv4 Internet protocols (value 2 on Linux). */
#define B7188_AF_INET ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7188_sock_is_af_inet(uint32_t u32Domain)
{
	return (u32Domain == B7188_AF_INET) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sock_is_af_inet_u_7188 - 1 if domain is AF_INET.
 *
 * domain: socket(2) domain / address-family argument
 *
 * Returns 1 when domain equals AF_INET (2), else 0.
 * Pure integer compare; does not call socket. No parent wires.
 */
uint32_t
gj_sock_is_af_inet_u_7188(uint32_t u32Domain)
{
	(void)NULL;
	return b7188_sock_is_af_inet(u32Domain);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sock_is_af_inet_u_7188(uint32_t u32Domain)
    __attribute__((alias("gj_sock_is_af_inet_u_7188")));
