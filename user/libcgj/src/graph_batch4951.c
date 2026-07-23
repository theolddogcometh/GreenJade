/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4951: TLS ContentType application_data constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_tls_content_appdata_u(void);
 *     - Returns the TLS ContentType value for application_data (23 /
 *       0x17 per RFC 5246 §6.2.1 / RFC 8446 §5.1).
 *   uint32_t __gj_tls_content_appdata_u  (alias)
 *   __libcgj_batch4951_marker = "libcgj-batch4951"
 *
 * Exclusive continuum CREATE-ONLY (4951-4960: TLS record unique —
 * content_appdata_u, content_handshake_u, content_alert_u,
 * content_ccs_u, is_appdata_u, rec_overhead_u, max_plain_u,
 * version_12_u, version_13_u, batch_id_4960). Distinct from
 * gj_tls_record_header_parse/build (batch136) and gj_tls_rec_parse
 * (batch343) — constant surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4951_marker[] = "libcgj-batch4951";

/* TLS ContentType application_data (RFC 5246 / RFC 8446). */
#define B4951_TLS_CT_APPDATA  23u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4951_content_appdata(void)
{
	return B4951_TLS_CT_APPDATA;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tls_content_appdata_u - TLS ContentType application_data (23).
 *
 * Always returns 23. Self-contained; no parent wires.
 */
uint32_t
gj_tls_content_appdata_u(void)
{
	(void)NULL;
	return b4951_content_appdata();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tls_content_appdata_u(void)
    __attribute__((alias("gj_tls_content_appdata_u")));
