/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9241: soft OpenSSL/TLS 1.2 ProtocolVersion id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ssl_tls12_id_9241(void);
 *     - Returns 0x0303 (TLS 1.2 ProtocolVersion wire id, RFC 5246).
 *       Soft catalog constant only; no crypto, handshake, or I/O.
 *   uint32_t __gj_ssl_tls12_id_9241  (alias)
 *   __libcgj_batch9241_marker = "libcgj-batch9241"
 *
 * Exclusive continuum CREATE-ONLY (9241-9250: openssl soft id stubs —
 * ssl_tls12_id_9241, ssl_tls13_id_9242, ssl_ctx_ok_u_9243,
 * ssl_cert_ok_u_9244, ssl_key_ok_u_9245, ssl_verify_ok_u_9246,
 * ssl_handshake_ok_u_9247, ssl_cipher_ok_u_9248, ssl_ready_u_9249,
 * batch_id_9250). Unique surface only; no multi-def. Distinct from
 * gj_tls_version_12_id_8881 (batch8881). No parent wires. No __int128.
 * No crypto implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9241_marker[] = "libcgj-batch9241";

/* Soft TLS 1.2 ProtocolVersion catalog id (RFC 5246). */
#define B9241_TLS12_ID  0x0303u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9241_tls12_id(void)
{
	return B9241_TLS12_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ssl_tls12_id_9241 - soft OpenSSL/TLS 1.2 ProtocolVersion catalog id.
 *
 * Always returns 0x0303. Soft continuum constant; does not negotiate
 * versions or perform crypto. No parent wires.
 */
uint32_t
gj_ssl_tls12_id_9241(void)
{
	(void)NULL;
	return b9241_tls12_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ssl_tls12_id_9241(void)
    __attribute__((alias("gj_ssl_tls12_id_9241")));
