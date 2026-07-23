/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4955: TLS ContentType application_data predicate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_tls_is_appdata_u(uint32_t t);
 *     - Returns 1 when t equals the TLS ContentType application_data
 *       (23), else 0. Pure integer predicate; no parse side effects.
 *   uint32_t __gj_tls_is_appdata_u  (alias)
 *   __libcgj_batch4955_marker = "libcgj-batch4955"
 *
 * Exclusive continuum CREATE-ONLY (4951-4960: TLS record unique).
 * Distinct from gj_tls_content_appdata_u (batch4951) — predicate on
 * an input type, not a bare constant. No multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4955_marker[] = "libcgj-batch4955";

/* TLS ContentType application_data (RFC 5246 / RFC 8446). */
#define B4955_TLS_CT_APPDATA  23u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4955_is_appdata(uint32_t u32T)
{
	if (u32T == B4955_TLS_CT_APPDATA) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tls_is_appdata_u - 1 if t is TLS ContentType application_data.
 *
 * t: candidate ContentType octet (widened to uint32_t)
 *
 * Returns 1 when t == 23, else 0. Self-contained; no parent wires.
 */
uint32_t
gj_tls_is_appdata_u(uint32_t u32T)
{
	(void)NULL;
	return b4955_is_appdata(u32T);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tls_is_appdata_u(uint32_t u32T)
    __attribute__((alias("gj_tls_is_appdata_u")));
