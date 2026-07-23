/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9247: soft OpenSSL handshake ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ssl_handshake_ok_u_9247(void);
 *     - Returns 1 (handshake soft-id continuum ok). Pure-data product
 *       tag; does not run TLS handshakes or exchange records.
 *   uint32_t __gj_ssl_handshake_ok_u_9247  (alias)
 *   __libcgj_batch9247_marker = "libcgj-batch9247"
 *
 * Exclusive continuum CREATE-ONLY (9241-9250: openssl soft id stubs —
 * ssl_tls12_id_9241, ssl_tls13_id_9242, ssl_ctx_ok_u_9243,
 * ssl_cert_ok_u_9244, ssl_key_ok_u_9245, ssl_verify_ok_u_9246,
 * ssl_handshake_ok_u_9247, ssl_cipher_ok_u_9248, ssl_ready_u_9249,
 * batch_id_9250). Unique surface only; no multi-def. Distinct from
 * gj_tls_handshake_ready_u_8883 (batch8883). No parent wires.
 * No __int128. No crypto implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9247_marker[] = "libcgj-batch9247";

#define B9247_HANDSHAKE_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9247_handshake_ok(void)
{
	return B9247_HANDSHAKE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ssl_handshake_ok_u_9247 - OpenSSL handshake soft-id continuum ok.
 *
 * Always returns 1. Soft pure-data product tag; does not run handshakes
 * or call OpenSSL/libc. No parent wires.
 */
uint32_t
gj_ssl_handshake_ok_u_9247(void)
{
	(void)NULL;
	return b9247_handshake_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ssl_handshake_ok_u_9247(void)
    __attribute__((alias("gj_ssl_handshake_ok_u_9247")));
