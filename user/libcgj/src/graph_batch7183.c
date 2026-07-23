/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7183: socket AF_INET6 domain id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_af_inet6_id_7183(void);
 *     - Return soft AF_INET6 constant (10).
 *   uint32_t __gj_af_inet6_id_7183  (alias)
 *   __libcgj_batch7183_marker = "libcgj-batch7183"
 *
 * Exclusive continuum CREATE-ONLY (7181-7190: socket domain/type stubs —
 * af_unix_id, af_inet_id, af_inet6_id, af_packet_id, sock_stream_id,
 * sock_dgram_id, sock_raw_id, is_af_inet, domain_type_ok, batch_id_7190).
 * Unique gj_af_inet6_id_7183 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7183_marker[] = "libcgj-batch7183";

/* AF_INET6: IPv6 Internet protocols (value 10 on Linux). */
#define B7183_AF_INET6 ((uint32_t)10u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7183_af_inet6_id(void)
{
	return B7183_AF_INET6;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_af_inet6_id_7183 - soft AF_INET6 domain constant.
 *
 * Always returns 10 (Linux AF_INET6). Catalog id only; does not call
 * socket. No parent wires.
 */
uint32_t
gj_af_inet6_id_7183(void)
{
	(void)NULL;
	return b7183_af_inet6_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_af_inet6_id_7183(void)
    __attribute__((alias("gj_af_inet6_id_7183")));
