/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8877: soft DNS TYPE PTR RR type id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_dns_ptr_id_8877(void);
 *     - Returns 12 (DNS TYPE PTR soft catalog value). Pure-data RR
 *       type id stub; does not issue queries or call resolv/libc.
 *   uint32_t __gj_dns_ptr_id_8877  (alias)
 *   __libcgj_batch8877_marker = "libcgj-batch8877"
 *
 * Exclusive continuum CREATE-ONLY (8871-8880: resolv/dns id stubs).
 * Unique gj_dns_ptr_id_8877 surface only; no multi-def.
 * Distinct from NS (batch8876) and gj_dns_ptr_offset (batch1754).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8877_marker[] = "libcgj-batch8877";

/* Soft DNS TYPE PTR catalog value (RR type 12). */
#define B8877_DNS_PTR  12u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8877_ptr_id(void)
{
	return B8877_DNS_PTR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dns_ptr_id_8877 - soft DNS TYPE PTR RR type catalog id.
 *
 * Always returns 12. Soft continuum constant; does not issue queries.
 * No parent wires.
 */
uint32_t
gj_dns_ptr_id_8877(void)
{
	(void)NULL;
	return b8877_ptr_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dns_ptr_id_8877(void)
    __attribute__((alias("gj_dns_ptr_id_8877")));
