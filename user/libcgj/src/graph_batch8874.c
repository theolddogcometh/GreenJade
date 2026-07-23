/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8874: soft DNS TYPE MX RR type id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_dns_mx_id_8874(void);
 *     - Returns 15 (DNS TYPE MX soft catalog value). Pure-data RR
 *       type id stub; does not issue queries or call resolv/libc.
 *   uint32_t __gj_dns_mx_id_8874  (alias)
 *   __libcgj_batch8874_marker = "libcgj-batch8874"
 *
 * Exclusive continuum CREATE-ONLY (8871-8880: resolv/dns id stubs).
 * Unique gj_dns_mx_id_8874 surface only; no multi-def.
 * Distinct from CNAME (batch8873). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8874_marker[] = "libcgj-batch8874";

/* Soft DNS TYPE MX catalog value (RR type 15). */
#define B8874_DNS_MX  15u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8874_mx_id(void)
{
	return B8874_DNS_MX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dns_mx_id_8874 - soft DNS TYPE MX RR type catalog id.
 *
 * Always returns 15. Soft continuum constant; does not issue queries.
 * No parent wires.
 */
uint32_t
gj_dns_mx_id_8874(void)
{
	(void)NULL;
	return b8874_mx_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dns_mx_id_8874(void)
    __attribute__((alias("gj_dns_mx_id_8874")));
