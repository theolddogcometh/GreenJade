/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7186: socket SOCK_DGRAM type id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sock_dgram_id_7186(void);
 *     - Return soft SOCK_DGRAM constant (2).
 *   uint32_t __gj_sock_dgram_id_7186  (alias)
 *   __libcgj_batch7186_marker = "libcgj-batch7186"
 *
 * Exclusive continuum CREATE-ONLY (7181-7190: socket domain/type stubs —
 * af_unix_id, af_inet_id, af_inet6_id, af_packet_id, sock_stream_id,
 * sock_dgram_id, sock_raw_id, is_af_inet, domain_type_ok, batch_id_7190).
 * Unique gj_sock_dgram_id_7186 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7186_marker[] = "libcgj-batch7186";

/* SOCK_DGRAM: connectionless, unreliable datagrams (value 2). */
#define B7186_SOCK_DGRAM ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7186_sock_dgram_id(void)
{
	return B7186_SOCK_DGRAM;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sock_dgram_id_7186 - soft SOCK_DGRAM type constant.
 *
 * Always returns 2 (Linux SOCK_DGRAM). Catalog id only; does not call
 * socket. No parent wires.
 */
uint32_t
gj_sock_dgram_id_7186(void)
{
	(void)NULL;
	return b7186_sock_dgram_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sock_dgram_id_7186(void)
    __attribute__((alias("gj_sock_dgram_id_7186")));
