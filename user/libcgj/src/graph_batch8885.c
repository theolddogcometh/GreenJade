/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8885: soft TLS alert close_notify description id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_tls_alert_close_id_8885(void);
 *     - Returns 0 (TLS AlertDescription close_notify, RFC 5246/8446).
 *       Soft catalog constant only; does not send alerts or close
 *       sockets.
 *   uint32_t __gj_tls_alert_close_id_8885  (alias)
 *   __libcgj_batch8885_marker = "libcgj-batch8885"
 *
 * Exclusive continuum CREATE-ONLY (8881-8890: tls/ssl soft id stubs —
 * tls_version_12_id_8881, tls_version_13_id_8882,
 * tls_handshake_ready_u_8883, tls_record_ok_u_8884,
 * tls_alert_close_id_8885, tls_cipher_ok_u_8886, tls_sni_ok_u_8887,
 * tls_alpn_ok_u_8888, tls_ready_u_8889, batch_id_8890).
 * Unique surface only; no multi-def. Distinct from gj_tls_alert_parse
 * (batch193). No parent wires. No __int128. No crypto implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8885_marker[] = "libcgj-batch8885";

/* Soft TLS AlertDescription close_notify catalog id. */
#define B8885_ALERT_CLOSE_ID  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8885_alert_close_id(void)
{
	return B8885_ALERT_CLOSE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tls_alert_close_id_8885 - soft TLS close_notify AlertDescription id.
 *
 * Always returns 0. Soft continuum constant; does not emit alerts or
 * close connections. No parent wires.
 */
uint32_t
gj_tls_alert_close_id_8885(void)
{
	(void)NULL;
	return b8885_alert_close_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tls_alert_close_id_8885(void)
    __attribute__((alias("gj_tls_alert_close_id_8885")));
