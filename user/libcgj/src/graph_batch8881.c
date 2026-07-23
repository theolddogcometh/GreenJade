/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8881: soft TLS 1.2 ProtocolVersion id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_tls_version_12_id_8881(void);
 *     - Returns 0x0303 (TLS 1.2 ProtocolVersion wire id, RFC 5246).
 *       Soft catalog constant only; no crypto, handshake, or I/O.
 *   uint32_t __gj_tls_version_12_id_8881  (alias)
 *   __libcgj_batch8881_marker = "libcgj-batch8881"
 *
 * Exclusive continuum CREATE-ONLY (8881-8890: tls/ssl soft id stubs —
 * tls_version_12_id_8881, tls_version_13_id_8882,
 * tls_handshake_ready_u_8883, tls_record_ok_u_8884,
 * tls_alert_close_id_8885, tls_cipher_ok_u_8886, tls_sni_ok_u_8887,
 * tls_alpn_ok_u_8888, tls_ready_u_8889, batch_id_8890).
 * Unique surface only; no multi-def. Distinct from gj_tls_version_12_u
 * (batch4958). No parent wires. No __int128. No crypto implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8881_marker[] = "libcgj-batch8881";

/* Soft TLS 1.2 ProtocolVersion catalog id (RFC 5246). */
#define B8881_TLS12_ID  0x0303u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8881_tls12_id(void)
{
	return B8881_TLS12_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tls_version_12_id_8881 - soft TLS 1.2 ProtocolVersion catalog id.
 *
 * Always returns 0x0303. Soft continuum constant; does not negotiate
 * versions or perform crypto. No parent wires.
 */
uint32_t
gj_tls_version_12_id_8881(void)
{
	(void)NULL;
	return b8881_tls12_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tls_version_12_id_8881(void)
    __attribute__((alias("gj_tls_version_12_id_8881")));
