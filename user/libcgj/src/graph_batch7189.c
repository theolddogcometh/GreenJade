/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7189: socket domain/type soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sock_domain_type_ok_u_7189(uint32_t domain, uint32_t type);
 *     - Return 1 if domain is a known soft AF and type is a known soft
 *       SOCK_* (base type bits only), else 0.
 *   uint32_t __gj_sock_domain_type_ok_u_7189  (alias)
 *   __libcgj_batch7189_marker = "libcgj-batch7189"
 *
 * Exclusive continuum CREATE-ONLY (7181-7190: socket domain/type stubs —
 * af_unix_id, af_inet_id, af_inet6_id, af_packet_id, sock_stream_id,
 * sock_dgram_id, sock_raw_id, is_af_inet, domain_type_ok, batch_id_7190).
 * Unique gj_sock_domain_type_ok_u_7189 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7189_marker[] = "libcgj-batch7189";

/*
 * Soft catalog domains (Linux):
 *   AF_UNIX=1, AF_INET=2, AF_INET6=10, AF_PACKET=17
 * Soft catalog types (base, ignoring SOCK_CLOEXEC/NONBLOCK flags):
 *   SOCK_STREAM=1, SOCK_DGRAM=2, SOCK_RAW=3
 * Linux type flags live in high bits; mask them for soft ok.
 */
#define B7189_AF_UNIX   ((uint32_t)1u)
#define B7189_AF_INET   ((uint32_t)2u)
#define B7189_AF_INET6  ((uint32_t)10u)
#define B7189_AF_PACKET ((uint32_t)17u)
#define B7189_SOCK_TYPE_MASK ((uint32_t)0xFu)
#define B7189_SOCK_STREAM ((uint32_t)1u)
#define B7189_SOCK_DGRAM  ((uint32_t)2u)
#define B7189_SOCK_RAW    ((uint32_t)3u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7189_domain_ok(uint32_t u32Domain)
{
	return (u32Domain == B7189_AF_UNIX ||
	        u32Domain == B7189_AF_INET ||
	        u32Domain == B7189_AF_INET6 ||
	        u32Domain == B7189_AF_PACKET) ? 1u : 0u;
}

static uint32_t
b7189_type_ok(uint32_t u32Type)
{
	uint32_t u32Base;

	u32Base = u32Type & B7189_SOCK_TYPE_MASK;
	return (u32Base == B7189_SOCK_STREAM ||
	        u32Base == B7189_SOCK_DGRAM ||
	        u32Base == B7189_SOCK_RAW) ? 1u : 0u;
}

static uint32_t
b7189_domain_type_ok(uint32_t u32Domain, uint32_t u32Type)
{
	return (b7189_domain_ok(u32Domain) != 0u &&
	        b7189_type_ok(u32Type) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sock_domain_type_ok_u_7189 - 1 if domain+type are soft-known.
 *
 * domain: socket(2) domain / address-family argument
 * type:   socket(2) type argument (base bits; high flags ignored)
 *
 * Returns 1 when domain is in {AF_UNIX, AF_INET, AF_INET6, AF_PACKET}
 * and base type is in {SOCK_STREAM, SOCK_DGRAM, SOCK_RAW}, else 0.
 * Soft catalog check; does not call socket. No parent wires.
 */
uint32_t
gj_sock_domain_type_ok_u_7189(uint32_t u32Domain, uint32_t u32Type)
{
	(void)NULL;
	return b7189_domain_type_ok(u32Domain, u32Type);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sock_domain_type_ok_u_7189(uint32_t u32Domain, uint32_t u32Type)
    __attribute__((alias("gj_sock_domain_type_ok_u_7189")));
