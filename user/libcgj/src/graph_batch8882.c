/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8882: soft TLS 1.3 ProtocolVersion id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_tls_version_13_id_8882(void);
 *     - Returns 0x0304 (TLS 1.3 ProtocolVersion id, RFC 8446). Soft
 *       catalog constant only; no crypto, handshake, or I/O. Note:
 *       on-the-wire record headers for TLS 1.3 still often carry
 *       0x0303 for compatibility; this is the negotiated id.
 *   uint32_t __gj_tls_version_13_id_8882  (alias)
 *   __libcgj_batch8882_marker = "libcgj-batch8882"
 *
 * Exclusive continuum CREATE-ONLY (8881-8890: tls/ssl soft id stubs —
 * tls_version_12_id_8881, tls_version_13_id_8882,
 * tls_handshake_ready_u_8883, tls_record_ok_u_8884,
 * tls_alert_close_id_8885, tls_cipher_ok_u_8886, tls_sni_ok_u_8887,
 * tls_alpn_ok_u_8888, tls_ready_u_8889, batch_id_8890).
 * Unique surface only; no multi-def. Distinct from gj_tls_version_13_u
 * (batch4959). No parent wires. No __int128. No crypto implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8882_marker[] = "libcgj-batch8882";

/* Soft TLS 1.3 ProtocolVersion catalog id (RFC 8446). */
#define B8882_TLS13_ID  0x0304u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8882_tls13_id(void)
{
	return B8882_TLS13_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tls_version_13_id_8882 - soft TLS 1.3 ProtocolVersion catalog id.
 *
 * Always returns 0x0304. Soft continuum constant; does not negotiate
 * versions or perform crypto. No parent wires.
 */
uint32_t
gj_tls_version_13_id_8882(void)
{
	(void)NULL;
	return b8882_tls13_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tls_version_13_id_8882(void)
    __attribute__((alias("gj_tls_version_13_id_8882")));
