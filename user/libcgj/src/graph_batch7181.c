/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7181: socket AF_UNIX domain id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_af_unix_id_7181(void);
 *     - Return soft AF_UNIX / AF_LOCAL constant (1).
 *   uint32_t __gj_af_unix_id_7181  (alias)
 *   __libcgj_batch7181_marker = "libcgj-batch7181"
 *
 * Exclusive continuum CREATE-ONLY (7181-7190: socket domain/type stubs —
 * af_unix_id, af_inet_id, af_inet6_id, af_packet_id, sock_stream_id,
 * sock_dgram_id, sock_raw_id, is_af_inet, domain_type_ok, batch_id_7190).
 * Unique gj_af_unix_id_7181 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7181_marker[] = "libcgj-batch7181";

/* AF_UNIX / AF_LOCAL: local communication (value 1 on Linux). */
#define B7181_AF_UNIX ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7181_af_unix_id(void)
{
	return B7181_AF_UNIX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_af_unix_id_7181 - soft AF_UNIX domain constant.
 *
 * Always returns 1 (Linux AF_UNIX / AF_LOCAL). Catalog id only; does not
 * call socket. No parent wires.
 */
uint32_t
gj_af_unix_id_7181(void)
{
	(void)NULL;
	return b7181_af_unix_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_af_unix_id_7181(void)
    __attribute__((alias("gj_af_unix_id_7181")));
