/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4952: TLS ContentType handshake constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_tls_content_handshake_u(void);
 *     - Returns the TLS ContentType value for handshake (22 / 0x16 per
 *       RFC 5246 §6.2.1 / RFC 8446 §5.1).
 *   uint32_t __gj_tls_content_handshake_u  (alias)
 *   __libcgj_batch4952_marker = "libcgj-batch4952"
 *
 * Exclusive continuum CREATE-ONLY (4951-4960: TLS record unique).
 * Distinct from gj_tls_record_header_parse/build (batch136) —
 * constant surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4952_marker[] = "libcgj-batch4952";

/* TLS ContentType handshake (RFC 5246 / RFC 8446). */
#define B4952_TLS_CT_HANDSHAKE  22u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4952_content_handshake(void)
{
	return B4952_TLS_CT_HANDSHAKE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tls_content_handshake_u - TLS ContentType handshake (22).
 *
 * Always returns 22. Self-contained; no parent wires.
 */
uint32_t
gj_tls_content_handshake_u(void)
{
	(void)NULL;
	return b4952_content_handshake();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tls_content_handshake_u(void)
    __attribute__((alias("gj_tls_content_handshake_u")));
