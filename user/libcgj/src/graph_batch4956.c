/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4956: TLS record header overhead constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_tls_rec_overhead_u(void);
 *     - Returns the fixed TLS record-layer header overhead in octets:
 *       ContentType(1) + ProtocolVersion(2) + length(2) = 5
 *       (RFC 5246 §6.2 / RFC 8446 §5.1).
 *   uint32_t __gj_tls_rec_overhead_u  (alias)
 *   __libcgj_batch4956_marker = "libcgj-batch4956"
 *
 * Exclusive continuum CREATE-ONLY (4951-4960: TLS record unique).
 * Distinct from gj_tls_record_header_len (batch1761, size_t) — unique
 * uint32_t gj_tls_rec_overhead_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4956_marker[] = "libcgj-batch4956";

/* Fixed TLS record header overhead in octets. */
#define B4956_TLS_REC_OVERHEAD  5u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4956_rec_overhead(void)
{
	return B4956_TLS_REC_OVERHEAD;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tls_rec_overhead_u - TLS record header overhead in octets (5).
 *
 * Always returns 5. Self-contained; no parent wires.
 */
uint32_t
gj_tls_rec_overhead_u(void)
{
	(void)NULL;
	return b4956_rec_overhead();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tls_rec_overhead_u(void)
    __attribute__((alias("gj_tls_rec_overhead_u")));
