/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4954: TLS ContentType change_cipher_spec.
 *
 * Surface (unique symbols):
 *   uint32_t gj_tls_content_ccs_u(void);
 *     - Returns the TLS ContentType value for change_cipher_spec (20 /
 *       0x14 per RFC 5246 §6.2.1). Still observed on the wire for TLS
 *       1.3 middlebox compatibility (RFC 8446 §5).
 *   uint32_t __gj_tls_content_ccs_u  (alias)
 *   __libcgj_batch4954_marker = "libcgj-batch4954"
 *
 * Exclusive continuum CREATE-ONLY (4951-4960: TLS record unique).
 * Constant surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4954_marker[] = "libcgj-batch4954";

/* TLS ContentType change_cipher_spec (RFC 5246). */
#define B4954_TLS_CT_CCS  20u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4954_content_ccs(void)
{
	return B4954_TLS_CT_CCS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tls_content_ccs_u - TLS ContentType change_cipher_spec (20).
 *
 * Always returns 20. Self-contained; no parent wires.
 */
uint32_t
gj_tls_content_ccs_u(void)
{
	(void)NULL;
	return b4954_content_ccs();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tls_content_ccs_u(void)
    __attribute__((alias("gj_tls_content_ccs_u")));
