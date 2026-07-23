/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8878: soft DNS/UDP well-known port id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_dns_port_id_8878(void);
 *     - Returns 53 (DNS service port soft catalog value). Pure-data
 *       port id stub; does not open sockets or call resolv/libc.
 *   uint32_t __gj_dns_port_id_8878  (alias)
 *   __libcgj_batch8878_marker = "libcgj-batch8878"
 *
 * Exclusive continuum CREATE-ONLY (8871-8880: resolv/dns id stubs).
 * Unique gj_dns_port_id_8878 surface only; no multi-def.
 * Distinct from RR type ids (batch8871-8877). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8878_marker[] = "libcgj-batch8878";

/* Soft DNS well-known port catalog value (UDP/TCP 53). */
#define B8878_DNS_PORT  53u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8878_port_id(void)
{
	return B8878_DNS_PORT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dns_port_id_8878 - soft DNS service port catalog id.
 *
 * Always returns 53. Soft continuum constant; does not bind or connect.
 * No parent wires.
 */
uint32_t
gj_dns_port_id_8878(void)
{
	(void)NULL;
	return b8878_port_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dns_port_id_8878(void)
    __attribute__((alias("gj_dns_port_id_8878")));
