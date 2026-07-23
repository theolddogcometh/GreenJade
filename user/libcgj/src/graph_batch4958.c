/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4958: TLS ProtocolVersion 1.2 constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_tls_version_12_u(void);
 *     - Returns the TLS ProtocolVersion wire value for TLS 1.2:
 *       0x0303 (RFC 5246).
 *   uint32_t __gj_tls_version_12_u  (alias)
 *   __libcgj_batch4958_marker = "libcgj-batch4958"
 *
 * Exclusive continuum CREATE-ONLY (4951-4960: TLS record unique).
 * Constant surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4958_marker[] = "libcgj-batch4958";

/* TLS ProtocolVersion for TLS 1.2 (RFC 5246). */
#define B4958_TLS_VERSION_12  0x0303u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4958_version_12(void)
{
	return B4958_TLS_VERSION_12;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tls_version_12_u - TLS ProtocolVersion for TLS 1.2 (0x0303).
 *
 * Always returns 0x0303. Self-contained; no parent wires.
 */
uint32_t
gj_tls_version_12_u(void)
{
	(void)NULL;
	return b4958_version_12();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tls_version_12_u(void)
    __attribute__((alias("gj_tls_version_12_u")));
