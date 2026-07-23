/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4933: socket address family constant AF_UNIX.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sockaddr_family_unix_u(void);
 *     - Returns the Linux/POSIX AF_UNIX / AF_LOCAL address-family
 *       constant (1).
 *   uint32_t __gj_sockaddr_family_unix_u  (alias)
 *   __libcgj_batch4933_marker = "libcgj-batch4933"
 *
 * Exclusive continuum CREATE-ONLY socket-addr wave (4931-4940). Unique
 * gj_sockaddr_family_unix_u surface only; no multi-def. Distinct from
 * gj_sockaddr_family_inet_u (batch4931) and gj_sockaddr_family_inet6_u
 * (batch4932). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4933_marker[] = "libcgj-batch4933";

/* Linux AF_UNIX / AF_LOCAL value (sys/socket.h). */
#define B4933_AF_UNIX  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4933_af_unix(void)
{
	return B4933_AF_UNIX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sockaddr_family_unix_u - report AF_UNIX address-family constant.
 *
 * Always returns 1 (Linux/POSIX AF_UNIX). Soft compile-time tag for
 * freestanding code that must not include <sys/socket.h>. No parent
 * wires.
 */
uint32_t
gj_sockaddr_family_unix_u(void)
{
	(void)NULL;
	return b4933_af_unix();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sockaddr_family_unix_u(void)
    __attribute__((alias("gj_sockaddr_family_unix_u")));
