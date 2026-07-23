/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8873: soft DNS TYPE CNAME RR type id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_dns_cname_id_8873(void);
 *     - Returns 5 (DNS TYPE CNAME soft catalog value). Pure-data RR
 *       type id stub; does not issue queries or call resolv/libc.
 *   uint32_t __gj_dns_cname_id_8873  (alias)
 *   __libcgj_batch8873_marker = "libcgj-batch8873"
 *
 * Exclusive continuum CREATE-ONLY (8871-8880: resolv/dns id stubs).
 * Unique gj_dns_cname_id_8873 surface only; no multi-def.
 * Distinct from A/AAAA (batch8871/8872). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8873_marker[] = "libcgj-batch8873";

/* Soft DNS TYPE CNAME catalog value (RR type 5). */
#define B8873_DNS_CNAME  5u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8873_cname_id(void)
{
	return B8873_DNS_CNAME;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dns_cname_id_8873 - soft DNS TYPE CNAME RR type catalog id.
 *
 * Always returns 5. Soft continuum constant; does not issue queries.
 * No parent wires.
 */
uint32_t
gj_dns_cname_id_8873(void)
{
	(void)NULL;
	return b8873_cname_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dns_cname_id_8873(void)
    __attribute__((alias("gj_dns_cname_id_8873")));
