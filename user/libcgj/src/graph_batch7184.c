/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7184: socket AF_PACKET domain id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_af_packet_id_7184(void);
 *     - Return soft AF_PACKET constant (17).
 *   uint32_t __gj_af_packet_id_7184  (alias)
 *   __libcgj_batch7184_marker = "libcgj-batch7184"
 *
 * Exclusive continuum CREATE-ONLY (7181-7190: socket domain/type stubs —
 * af_unix_id, af_inet_id, af_inet6_id, af_packet_id, sock_stream_id,
 * sock_dgram_id, sock_raw_id, is_af_inet, domain_type_ok, batch_id_7190).
 * Unique gj_af_packet_id_7184 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7184_marker[] = "libcgj-batch7184";

/* AF_PACKET: low-level packet interface (value 17 on Linux). */
#define B7184_AF_PACKET ((uint32_t)17u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7184_af_packet_id(void)
{
	return B7184_AF_PACKET;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_af_packet_id_7184 - soft AF_PACKET domain constant.
 *
 * Always returns 17 (Linux AF_PACKET). Catalog id only; does not call
 * socket. No parent wires.
 */
uint32_t
gj_af_packet_id_7184(void)
{
	(void)NULL;
	return b7184_af_packet_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_af_packet_id_7184(void)
    __attribute__((alias("gj_af_packet_id_7184")));
