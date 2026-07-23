/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9245: soft OpenSSL private-key ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ssl_key_ok_u_9245(void);
 *     - Returns 1 (private-key soft-id continuum ok). Pure-data product
 *       tag; does not load, generate, or use private keys.
 *   uint32_t __gj_ssl_key_ok_u_9245  (alias)
 *   __libcgj_batch9245_marker = "libcgj-batch9245"
 *
 * Exclusive continuum CREATE-ONLY (9241-9250: openssl soft id stubs —
 * ssl_tls12_id_9241, ssl_tls13_id_9242, ssl_ctx_ok_u_9243,
 * ssl_cert_ok_u_9244, ssl_key_ok_u_9245, ssl_verify_ok_u_9246,
 * ssl_handshake_ok_u_9247, ssl_cipher_ok_u_9248, ssl_ready_u_9249,
 * batch_id_9250). Unique surface only; no multi-def. No parent wires.
 * No __int128. No crypto implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9245_marker[] = "libcgj-batch9245";

#define B9245_KEY_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9245_key_ok(void)
{
	return B9245_KEY_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ssl_key_ok_u_9245 - OpenSSL private-key soft-id continuum ok flag.
 *
 * Always returns 1. Soft pure-data product tag; does not load or use
 * keys or call OpenSSL/libc. No parent wires.
 */
uint32_t
gj_ssl_key_ok_u_9245(void)
{
	(void)NULL;
	return b9245_key_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ssl_key_ok_u_9245(void)
    __attribute__((alias("gj_ssl_key_ok_u_9245")));
