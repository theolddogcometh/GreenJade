/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8872: soft DNS TYPE AAAA (IPv6) RR type id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_dns_aaaa_id_8872(void);
 *     - Returns 28 (DNS TYPE AAAA soft catalog value). Pure-data RR
 *       type id stub; does not issue queries or call resolv/libc.
 *   uint32_t __gj_dns_aaaa_id_8872  (alias)
 *   __libcgj_batch8872_marker = "libcgj-batch8872"
 *
 * Exclusive continuum CREATE-ONLY (8871-8880: resolv/dns id stubs).
 * Unique gj_dns_aaaa_id_8872 surface only; no multi-def.
 * Distinct from A (batch8871). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8872_marker[] = "libcgj-batch8872";

/* Soft DNS TYPE AAAA catalog value (RR type 28). */
#define B8872_DNS_AAAA  28u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8872_aaaa_id(void)
{
	return B8872_DNS_AAAA;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dns_aaaa_id_8872 - soft DNS TYPE AAAA RR type catalog id.
 *
 * Always returns 28. Soft continuum constant; does not issue queries.
 * No parent wires.
 */
uint32_t
gj_dns_aaaa_id_8872(void)
{
	(void)NULL;
	return b8872_aaaa_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dns_aaaa_id_8872(void)
    __attribute__((alias("gj_dns_aaaa_id_8872")));
