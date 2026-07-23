/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1751: DNS wire label length validity.
 *
 * Surface (unique symbols):
 *   int gj_dns_label_len_ok(size_t len);
 *     — Return 1 if len is a legal single DNS label length (1..63
 *       octets per RFC 1035 §2.3.4). 0 and >63 → 0.
 *   int __gj_dns_label_len_ok  (alias)
 *   __libcgj_batch1751_marker = "libcgj-batch1751"
 *
 * Distinct from gj_dns_label_ok (batch1118) — length-only, no LDH
 * content check. DNS/wire exclusive lightweight wave (1751–1759).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1751_marker[] = "libcgj-batch1751";

/* ---- freestanding helpers ---------------------------------------------- */

/* True when cb is a legal on-wire label length (1..63). */
static int
b1751_label_len_ok(size_t cb)
{
	return (cb >= 1u && cb <= 63u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dns_label_len_ok — 1 if len is a valid DNS wire label length.
 *
 * cb: proposed label length in octets
 * Returns 1 when 1 <= cb <= 63, else 0.
 */
int
gj_dns_label_len_ok(size_t cb)
{
	return b1751_label_len_ok(cb);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_dns_label_len_ok(size_t cb)
    __attribute__((alias("gj_dns_label_len_ok")));
