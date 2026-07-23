/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4953: TLS ContentType alert constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_tls_content_alert_u(void);
 *     - Returns the TLS ContentType value for alert (21 / 0x15 per
 *       RFC 5246 §6.2.1 / RFC 8446 §5.1).
 *   uint32_t __gj_tls_content_alert_u  (alias)
 *   __libcgj_batch4953_marker = "libcgj-batch4953"
 *
 * Exclusive continuum CREATE-ONLY (4951-4960: TLS record unique).
 * Distinct from gj_tls_alert_parse (batch193) — ContentType constant
 * only, not alert-body parse; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4953_marker[] = "libcgj-batch4953";

/* TLS ContentType alert (RFC 5246 / RFC 8446). */
#define B4953_TLS_CT_ALERT  21u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4953_content_alert(void)
{
	return B4953_TLS_CT_ALERT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tls_content_alert_u - TLS ContentType alert (21).
 *
 * Always returns 21. Self-contained; no parent wires.
 */
uint32_t
gj_tls_content_alert_u(void)
{
	(void)NULL;
	return b4953_content_alert();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tls_content_alert_u(void)
    __attribute__((alias("gj_tls_content_alert_u")));
