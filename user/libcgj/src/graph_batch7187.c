/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7187: socket SOCK_RAW type id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sock_raw_id_7187(void);
 *     - Return soft SOCK_RAW constant (3).
 *   uint32_t __gj_sock_raw_id_7187  (alias)
 *   __libcgj_batch7187_marker = "libcgj-batch7187"
 *
 * Exclusive continuum CREATE-ONLY (7181-7190: socket domain/type stubs —
 * af_unix_id, af_inet_id, af_inet6_id, af_packet_id, sock_stream_id,
 * sock_dgram_id, sock_raw_id, is_af_inet, domain_type_ok, batch_id_7190).
 * Unique gj_sock_raw_id_7187 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7187_marker[] = "libcgj-batch7187";

/* SOCK_RAW: raw network protocol access (value 3). */
#define B7187_SOCK_RAW ((uint32_t)3u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7187_sock_raw_id(void)
{
	return B7187_SOCK_RAW;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sock_raw_id_7187 - soft SOCK_RAW type constant.
 *
 * Always returns 3 (Linux SOCK_RAW). Catalog id only; does not call
 * socket. No parent wires.
 */
uint32_t
gj_sock_raw_id_7187(void)
{
	(void)NULL;
	return b7187_sock_raw_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sock_raw_id_7187(void)
    __attribute__((alias("gj_sock_raw_id_7187")));
