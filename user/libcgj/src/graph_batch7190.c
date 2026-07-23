/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7190: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7190(void);
 *     - Returns the compile-time graph batch number for this TU (7190).
 *   uint32_t __gj_batch_id_7190  (alias)
 *   __libcgj_batch7190_marker = "libcgj-batch7190"
 *
 * Exclusive continuum CREATE-ONLY (7181-7190: socket domain/type stubs —
 * af_unix_id, af_inet_id, af_inet6_id, af_packet_id, sock_stream_id,
 * sock_dgram_id, sock_raw_id, is_af_inet, domain_type_ok, batch_id_7190).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7190_marker[] = "libcgj-batch7190";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7190_id(void)
{
	return 7190u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7190 - report this TU's graph batch number.
 *
 * Always returns 7190. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_7190(void)
{
	(void)NULL;
	return b7190_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_7190(void)
    __attribute__((alias("gj_batch_id_7190")));
