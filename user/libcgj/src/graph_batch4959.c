/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4959: TLS ProtocolVersion 1.3 constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_tls_version_13_u(void);
 *     - Returns the TLS ProtocolVersion wire value for TLS 1.3:
 *       0x0304 (RFC 8446). Note: on-the-wire record headers for TLS
 *       1.3 still often carry 0x0303 for compatibility; this constant
 *       is the negotiated protocol version, not the record-legacy
 *       version field.
 *   uint32_t __gj_tls_version_13_u  (alias)
 *   __libcgj_batch4959_marker = "libcgj-batch4959"
 *
 * Exclusive continuum CREATE-ONLY (4951-4960: TLS record unique).
 * Distinct from gj_tls_version_12_u (batch4958). Constant surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4959_marker[] = "libcgj-batch4959";

/* TLS ProtocolVersion for TLS 1.3 (RFC 8446). */
#define B4959_TLS_VERSION_13  0x0304u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4959_version_13(void)
{
	return B4959_TLS_VERSION_13;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tls_version_13_u - TLS ProtocolVersion for TLS 1.3 (0x0304).
 *
 * Always returns 0x0304. Self-contained; no parent wires.
 */
uint32_t
gj_tls_version_13_u(void)
{
	(void)NULL;
	return b4959_version_13();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tls_version_13_u(void)
    __attribute__((alias("gj_tls_version_13_u")));
