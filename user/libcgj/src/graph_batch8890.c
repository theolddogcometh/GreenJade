/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8890: tls/ssl soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8890(void);
 *     - Returns the compile-time graph batch number for this TU (8890).
 *   uint32_t __gj_batch_id_8890  (alias)
 *   __libcgj_batch8890_marker = "libcgj-batch8890"
 *
 * Exclusive continuum CREATE-ONLY (8881-8890: tls/ssl soft id stubs —
 * tls_version_12_id_8881, tls_version_13_id_8882,
 * tls_handshake_ready_u_8883, tls_record_ok_u_8884,
 * tls_alert_close_id_8885, tls_cipher_ok_u_8886, tls_sni_ok_u_8887,
 * tls_alpn_ok_u_8888, tls_ready_u_8889, batch_id_8890).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128. No crypto implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8890_marker[] = "libcgj-batch8890";

#define B8890_BATCH_ID  8890u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8890_id(void)
{
	return B8890_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8890 - report this TU's graph batch number.
 *
 * Always returns 8890.
 */
uint32_t
gj_batch_id_8890(void)
{
	(void)NULL;
	return b8890_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8890(void)
    __attribute__((alias("gj_batch_id_8890")));
