/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8876: soft DNS TYPE NS RR type id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_dns_ns_id_8876(void);
 *     - Returns 2 (DNS TYPE NS soft catalog value). Pure-data RR
 *       type id stub; does not issue queries or call resolv/libc.
 *   uint32_t __gj_dns_ns_id_8876  (alias)
 *   __libcgj_batch8876_marker = "libcgj-batch8876"
 *
 * Exclusive continuum CREATE-ONLY (8871-8880: resolv/dns id stubs).
 * Unique gj_dns_ns_id_8876 surface only; no multi-def.
 * Distinct from TXT (batch8875). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8876_marker[] = "libcgj-batch8876";

/* Soft DNS TYPE NS catalog value (RR type 2). */
#define B8876_DNS_NS  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8876_ns_id(void)
{
	return B8876_DNS_NS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dns_ns_id_8876 - soft DNS TYPE NS RR type catalog id.
 *
 * Always returns 2. Soft continuum constant; does not issue queries.
 * No parent wires.
 */
uint32_t
gj_dns_ns_id_8876(void)
{
	(void)NULL;
	return b8876_ns_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dns_ns_id_8876(void)
    __attribute__((alias("gj_dns_ns_id_8876")));
