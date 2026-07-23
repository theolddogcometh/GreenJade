/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1762: TLS max plaintext fragment hint.
 *
 * Surface (unique symbols):
 *   size_t gj_tls_max_plain_hint(void);
 *     — Conventional maximum TLSPlaintext.fragment length: 2^14 =
 *       16384 octets (RFC 5246 §6.2.1 / RFC 8446 §5.1). A size hint
 *       for buffer sizing; not a negotiated record-size limit.
 *   size_t __gj_tls_max_plain_hint  (alias)
 *   __libcgj_batch1762_marker = "libcgj-batch1762"
 *
 * TLS record / crypto size exclusive wave (1761–1769). Does NOT
 * redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1762_marker[] = "libcgj-batch1762";

/* TLSPlaintext fragment max (2^14). */
#define B1762_TLS_MAX_PLAIN  16384u

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b1762_max_plain(void)
{
	return (size_t)B1762_TLS_MAX_PLAIN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tls_max_plain_hint — max plaintext fragment size hint (16384).
 */
size_t
gj_tls_max_plain_hint(void)
{
	return b1762_max_plain();
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_tls_max_plain_hint(void)
    __attribute__((alias("gj_tls_max_plain_hint")));
