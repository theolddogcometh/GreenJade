/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4932: socket address family constant AF_INET6.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sockaddr_family_inet6_u(void);
 *     - Returns the Linux AF_INET6 address-family constant (10).
 *   uint32_t __gj_sockaddr_family_inet6_u  (alias)
 *   __libcgj_batch4932_marker = "libcgj-batch4932"
 *
 * Exclusive continuum CREATE-ONLY socket-addr wave (4931-4940). Unique
 * gj_sockaddr_family_inet6_u surface only; no multi-def. Distinct from
 * gj_sockaddr_family_inet_u (batch4931). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4932_marker[] = "libcgj-batch4932";

/* Linux AF_INET6 value (sys/socket.h). */
#define B4932_AF_INET6  10u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4932_af_inet6(void)
{
	return B4932_AF_INET6;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sockaddr_family_inet6_u - report AF_INET6 address-family constant.
 *
 * Always returns 10 (Linux AF_INET6). Soft compile-time tag for
 * freestanding code that must not include <sys/socket.h>. No parent
 * wires.
 */
uint32_t
gj_sockaddr_family_inet6_u(void)
{
	(void)NULL;
	return b4932_af_inet6();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sockaddr_family_inet6_u(void)
    __attribute__((alias("gj_sockaddr_family_inet6_u")));
