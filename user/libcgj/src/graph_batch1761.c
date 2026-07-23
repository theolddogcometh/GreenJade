/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1761: TLS record header length constant.
 *
 * Surface (unique symbols):
 *   size_t gj_tls_record_header_len(void);
 *     — Fixed TLS record-layer header size (RFC 5246 §6.2 / RFC 8446
 *       §5.1): ContentType(1) + ProtocolVersion(2) + length(2) = 5.
 *   size_t __gj_tls_record_header_len  (alias)
 *   __libcgj_batch1761_marker = "libcgj-batch1761"
 *
 * TLS record / crypto size exclusive wave (1761–1769). Distinct from
 * gj_tls_record_header_parse/build (batch136) — length constant only.
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1761_marker[] = "libcgj-batch1761";

/* Fixed TLS record header octet count. */
#define B1761_TLS_HDR_LEN  5u

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b1761_hdr_len(void)
{
	return (size_t)B1761_TLS_HDR_LEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tls_record_header_len — TLS record header size in octets (5).
 */
size_t
gj_tls_record_header_len(void)
{
	return b1761_hdr_len();
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_tls_record_header_len(void)
    __attribute__((alias("gj_tls_record_header_len")));
