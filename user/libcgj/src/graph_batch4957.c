/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4957: TLS max plaintext fragment constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_tls_max_plain_u(void);
 *     - Returns the conventional maximum TLSPlaintext.fragment length:
 *       2^14 = 16384 octets (RFC 5246 §6.2.1 / RFC 8446 §5.1).
 *   uint32_t __gj_tls_max_plain_u  (alias)
 *   __libcgj_batch4957_marker = "libcgj-batch4957"
 *
 * Exclusive continuum CREATE-ONLY (4951-4960: TLS record unique).
 * Distinct from gj_tls_max_plain_hint (batch1762, size_t) — unique
 * uint32_t gj_tls_max_plain_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4957_marker[] = "libcgj-batch4957";

/* TLSPlaintext fragment max (2^14). */
#define B4957_TLS_MAX_PLAIN  16384u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4957_max_plain(void)
{
	return B4957_TLS_MAX_PLAIN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tls_max_plain_u - max TLS plaintext fragment size (16384).
 *
 * Always returns 16384. Self-contained; no parent wires.
 */
uint32_t
gj_tls_max_plain_u(void)
{
	(void)NULL;
	return b4957_max_plain();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tls_max_plain_u(void)
    __attribute__((alias("gj_tls_max_plain_u")));
