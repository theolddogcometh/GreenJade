/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8875: soft DNS TYPE TXT RR type id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_dns_txt_id_8875(void);
 *     - Returns 16 (DNS TYPE TXT soft catalog value). Pure-data RR
 *       type id stub; does not issue queries or call resolv/libc.
 *   uint32_t __gj_dns_txt_id_8875  (alias)
 *   __libcgj_batch8875_marker = "libcgj-batch8875"
 *
 * Exclusive continuum CREATE-ONLY (8871-8880: resolv/dns id stubs).
 * Unique gj_dns_txt_id_8875 surface only; no multi-def.
 * Distinct from MX (batch8874). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8875_marker[] = "libcgj-batch8875";

/* Soft DNS TYPE TXT catalog value (RR type 16). */
#define B8875_DNS_TXT  16u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8875_txt_id(void)
{
	return B8875_DNS_TXT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dns_txt_id_8875 - soft DNS TYPE TXT RR type catalog id.
 *
 * Always returns 16. Soft continuum constant; does not issue queries.
 * No parent wires.
 */
uint32_t
gj_dns_txt_id_8875(void)
{
	(void)NULL;
	return b8875_txt_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dns_txt_id_8875(void)
    __attribute__((alias("gj_dns_txt_id_8875")));
