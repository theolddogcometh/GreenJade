/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3129: gj_net_ttl_ok_u - IPv4/IPv6 hop limit.
 *
 * Surface (unique symbols):
 *   int gj_net_ttl_ok_u(uint32_t ttl);
 *     - Return 1 if ttl fits an 8-bit IPv4 TTL / IPv6 Hop Limit field
 *       (0..255 inclusive). TTL 0 is accepted as a valid field value
 *       (do-not-forward / expired). Values above 255 return 0.
 *   int __gj_net_ttl_ok_u  (alias)
 *   __libcgj_batch3129_marker = "libcgj-batch3129"
 *
 * Exclusive checksum/net CREATE-ONLY wave (3121-3130). Unique
 * gj_net_ttl_ok_u surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3129_marker[] = "libcgj-batch3129";

#define B3129_TTL_MAX 255u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if ttl is in [0, 255]. */
static int
b3129_ttl_ok(uint32_t uTtl)
{
	if (uTtl > B3129_TTL_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_net_ttl_ok_u - soft check that ttl fits an 8-bit hop-limit field.
 *
 * ttl: candidate IPv4 TTL or IPv6 Hop Limit value
 *
 * Returns 1 when ttl is in [0, 255], else 0. No parent wires.
 */
int
gj_net_ttl_ok_u(uint32_t uTtl)
{
	(void)NULL;
	return b3129_ttl_ok(uTtl);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_net_ttl_ok_u(uint32_t uTtl)
    __attribute__((alias("gj_net_ttl_ok_u")));
