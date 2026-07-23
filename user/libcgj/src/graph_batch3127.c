/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3127: gj_net_port_ok_u - TCP/UDP port range.
 *
 * Surface (unique symbols):
 *   int gj_net_port_ok_u(uint32_t port);
 *     - Return 1 if port fits in a 16-bit TCP/UDP port field
 *       (0..65535 inclusive). Port 0 is accepted (bind-any / ephemeral
 *       request). Values above 65535 return 0.
 *   int __gj_net_port_ok_u  (alias)
 *   __libcgj_batch3127_marker = "libcgj-batch3127"
 *
 * Exclusive checksum/net CREATE-ONLY wave (3121-3130). Unique
 * gj_net_port_ok_u surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3127_marker[] = "libcgj-batch3127";

#define B3127_PORT_MAX 65535u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if port is in [0, 65535]. */
static int
b3127_port_ok(uint32_t uPort)
{
	if (uPort > B3127_PORT_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_net_port_ok_u - soft check that port fits a 16-bit port field.
 *
 * port: candidate TCP/UDP port number (host integer form)
 *
 * Returns 1 when port is in [0, 65535], else 0. No parent wires.
 */
int
gj_net_port_ok_u(uint32_t uPort)
{
	(void)NULL;
	return b3127_port_ok(uPort);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_net_port_ok_u(uint32_t uPort)
    __attribute__((alias("gj_net_port_ok_u")));
