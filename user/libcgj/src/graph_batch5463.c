/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5463: DNS stub label-length validity.
 *
 * Surface (unique symbols):
 *   int gj_dns_stub_label_len_ok_5463(uint32_t len);
 *     — Return 1 if len is in [1, 63] (RFC 1035 DNS label length),
 *       else 0. Soft DNS-stub layout hint only; no network I/O.
 *   int __gj_dns_stub_label_len_ok_5463  (alias)
 *   __libcgj_batch5463_marker = "libcgj-batch5463"
 *
 * Steam network/filesystem exclusive pure-data wave (5461–5470).
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5463_marker[] = "libcgj-batch5463";

/* DNS label length bounds (RFC 1035). */
#define B5463_LABEL_MIN  1u
#define B5463_LABEL_MAX  63u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b5463_label_ok(uint32_t uLen)
{
	if (uLen < B5463_LABEL_MIN) {
		return 0;
	}
	if (uLen > B5463_LABEL_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dns_stub_label_len_ok_5463 — 1 iff len is a valid DNS label length.
 *
 * len: proposed label octet count (not including length prefix byte).
 * Returns 1 for 1..63 inclusive, else 0.
 */
int
gj_dns_stub_label_len_ok_5463(uint32_t uLen)
{
	(void)NULL;
	return b5463_label_ok(uLen);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_dns_stub_label_len_ok_5463(uint32_t uLen)
    __attribute__((alias("gj_dns_stub_label_len_ok_5463")));
