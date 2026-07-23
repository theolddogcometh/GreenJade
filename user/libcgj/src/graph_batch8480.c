/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8480: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8480(void);
 *     - Returns the compile-time graph batch number for this TU (8480).
 *   uint32_t __gj_batch_id_8480  (alias)
 *   __libcgj_batch8480_marker = "libcgj-batch8480"
 *
 * Exclusive continuum CREATE-ONLY (8471-8480: ip/sockaddr id stubs —
 * af_inet_id, af_inet6_id, af_unix_id, sock_stream_id, sock_dgram_id,
 * ip4_any_ok_u, ip4_loop_ok_u, port_ok_u, ip_ver4_u, batch_id_8480).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8480_marker[] = "libcgj-batch8480";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8480_id(void)
{
	return 8480u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8480 - report this TU's graph batch number.
 *
 * Always returns 8480. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_8480(void)
{
	(void)NULL;
	return b8480_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8480(void)
    __attribute__((alias("gj_batch_id_8480")));
