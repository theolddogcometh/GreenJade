/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7185: socket SOCK_STREAM type id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sock_stream_id_7185(void);
 *     - Return soft SOCK_STREAM constant (1).
 *   uint32_t __gj_sock_stream_id_7185  (alias)
 *   __libcgj_batch7185_marker = "libcgj-batch7185"
 *
 * Exclusive continuum CREATE-ONLY (7181-7190: socket domain/type stubs —
 * af_unix_id, af_inet_id, af_inet6_id, af_packet_id, sock_stream_id,
 * sock_dgram_id, sock_raw_id, is_af_inet, domain_type_ok, batch_id_7190).
 * Unique gj_sock_stream_id_7185 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7185_marker[] = "libcgj-batch7185";

/* SOCK_STREAM: sequenced, reliable, two-way byte stream (value 1). */
#define B7185_SOCK_STREAM ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7185_sock_stream_id(void)
{
	return B7185_SOCK_STREAM;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sock_stream_id_7185 - soft SOCK_STREAM type constant.
 *
 * Always returns 1 (Linux SOCK_STREAM). Catalog id only; does not call
 * socket. No parent wires.
 */
uint32_t
gj_sock_stream_id_7185(void)
{
	(void)NULL;
	return b7185_sock_stream_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sock_stream_id_7185(void)
    __attribute__((alias("gj_sock_stream_id_7185")));
