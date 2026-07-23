/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7182: socket AF_INET domain id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_af_inet_id_7182(void);
 *     - Return soft AF_INET constant (2).
 *   uint32_t __gj_af_inet_id_7182  (alias)
 *   __libcgj_batch7182_marker = "libcgj-batch7182"
 *
 * Exclusive continuum CREATE-ONLY (7181-7190: socket domain/type stubs —
 * af_unix_id, af_inet_id, af_inet6_id, af_packet_id, sock_stream_id,
 * sock_dgram_id, sock_raw_id, is_af_inet, domain_type_ok, batch_id_7190).
 * Unique gj_af_inet_id_7182 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7182_marker[] = "libcgj-batch7182";

/* AF_INET: IPv4 Internet protocols (value 2 on Linux). */
#define B7182_AF_INET ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7182_af_inet_id(void)
{
	return B7182_AF_INET;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_af_inet_id_7182 - soft AF_INET domain constant.
 *
 * Always returns 2 (Linux AF_INET). Catalog id only; does not call
 * socket. No parent wires.
 */
uint32_t
gj_af_inet_id_7182(void)
{
	(void)NULL;
	return b7182_af_inet_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_af_inet_id_7182(void)
    __attribute__((alias("gj_af_inet_id_7182")));
