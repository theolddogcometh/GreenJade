/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1752: DNS wire name total length validity.
 *
 * Surface (unique symbols):
 *   int gj_dns_name_len_ok(size_t len);
 *     — Return 1 if len is a legal total DNS wire name length
 *       (1..255 octets including the root label 0, RFC 1035 §2.3.4 /
 *       §3.1). 0 and >255 → 0.
 *   int __gj_dns_name_len_ok  (alias)
 *   __libcgj_batch1752_marker = "libcgj-batch1752"
 *
 * Distinct from gj_dns_name_ok (batch1119) — length-only wire limit,
 * no presentation LDH checks. DNS/wire exclusive lightweight wave
 * (1751–1759).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1752_marker[] = "libcgj-batch1752";

/* ---- freestanding helpers ---------------------------------------------- */

/* True when cb is a legal total on-wire domain-name length (1..255). */
static int
b1752_name_len_ok(size_t cb)
{
	return (cb >= 1u && cb <= 255u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dns_name_len_ok — 1 if len is a valid DNS wire name total length.
 *
 * cb: total wire name size in octets (labels + lengths + root 0)
 * Returns 1 when 1 <= cb <= 255, else 0.
 */
int
gj_dns_name_len_ok(size_t cb)
{
	return b1752_name_len_ok(cb);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_dns_name_len_ok(size_t cb)
    __attribute__((alias("gj_dns_name_len_ok")));
