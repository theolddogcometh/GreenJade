/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8478: TCP/UDP port range ok predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_port_ok_u_8478(uint32_t port);
 *     - Return 1 if port fits in a 16-bit port field (0..65535); else 0.
 *   int __gj_port_ok_u_8478  (alias)
 *   __libcgj_batch8478_marker = "libcgj-batch8478"
 *
 * Exclusive continuum CREATE-ONLY (8471-8480: ip/sockaddr id stubs —
 * af_inet_id, af_inet6_id, af_unix_id, sock_stream_id, sock_dgram_id,
 * ip4_any_ok_u, ip4_loop_ok_u, port_ok_u, ip_ver4_u, batch_id_8480).
 * Unique gj_port_ok_u_8478 surface only; no multi-def. Soft port-field
 * width check only (includes 0 = ephemeral/any). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8478_marker[] = "libcgj-batch8478";

/* Max value of a 16-bit port field. */
#define B8478_PORT_MAX ((uint32_t)65535u)

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff u32Port is in 0..65535. */
static int
b8478_port_ok(uint32_t u32Port)
{
	if (u32Port <= B8478_PORT_MAX) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_port_ok_u_8478 - 1 iff port fits in a 16-bit port field.
 *
 * Edge cases:
 *   0..65535 → 1
 *   65536..UINT32_MAX → 0
 */
int
gj_port_ok_u_8478(uint32_t u32Port)
{
	(void)NULL;
	return b8478_port_ok(u32Port);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_port_ok_u_8478(uint32_t u32Port)
    __attribute__((alias("gj_port_ok_u_8478")));
