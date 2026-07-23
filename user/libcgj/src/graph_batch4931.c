/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4931: socket address family constant AF_INET.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sockaddr_family_inet_u(void);
 *     - Returns the Linux/POSIX AF_INET address-family constant (2).
 *   uint32_t __gj_sockaddr_family_inet_u  (alias)
 *   __libcgj_batch4931_marker = "libcgj-batch4931"
 *
 * Exclusive continuum CREATE-ONLY socket-addr wave (4931-4940:
 * sockaddr_family_inet_u, sockaddr_family_inet6_u,
 * sockaddr_family_unix_u, ip4_pack_u, ip4_a_u, ip4_b_u, ip4_c_u,
 * ip4_d_u, ip4_is_loopback_u, batch_id_4940). Unique
 * gj_sockaddr_family_inet_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4931_marker[] = "libcgj-batch4931";

/* Linux AF_INET value (sys/socket.h). */
#define B4931_AF_INET  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4931_af_inet(void)
{
	return B4931_AF_INET;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sockaddr_family_inet_u - report AF_INET address-family constant.
 *
 * Always returns 2 (Linux/POSIX AF_INET). Soft compile-time tag for
 * freestanding code that must not include <sys/socket.h>. No parent
 * wires.
 */
uint32_t
gj_sockaddr_family_inet_u(void)
{
	(void)NULL;
	return b4931_af_inet();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sockaddr_family_inet_u(void)
    __attribute__((alias("gj_sockaddr_family_inet_u")));
