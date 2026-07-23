/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8888: soft TLS ALPN-ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_tls_alpn_ok_u_8888(void);
 *     - Returns 1 (TLS ALPN soft-id continuum ok). Pure-data product
 *       tag; does not negotiate Application-Layer Protocol Negotiation.
 *   uint32_t __gj_tls_alpn_ok_u_8888  (alias)
 *   __libcgj_batch8888_marker = "libcgj-batch8888"
 *
 * Exclusive continuum CREATE-ONLY (8881-8890: tls/ssl soft id stubs —
 * tls_version_12_id_8881, tls_version_13_id_8882,
 * tls_handshake_ready_u_8883, tls_record_ok_u_8884,
 * tls_alert_close_id_8885, tls_cipher_ok_u_8886, tls_sni_ok_u_8887,
 * tls_alpn_ok_u_8888, tls_ready_u_8889, batch_id_8890).
 * Unique surface only; no multi-def. No parent wires. No __int128.
 * No crypto implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8888_marker[] = "libcgj-batch8888";

#define B8888_ALPN_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8888_alpn_ok(void)
{
	return B8888_ALPN_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tls_alpn_ok_u_8888 - TLS ALPN soft-id continuum ok flag.
 *
 * Always returns 1. Soft pure-data product tag; does not negotiate
 * ALPN or call libc. No parent wires.
 */
uint32_t
gj_tls_alpn_ok_u_8888(void)
{
	(void)NULL;
	return b8888_alpn_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tls_alpn_ok_u_8888(void)
    __attribute__((alias("gj_tls_alpn_ok_u_8888")));
