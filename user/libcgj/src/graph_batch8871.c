/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8871: soft DNS TYPE A (IPv4) RR type id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_dns_a_id_8871(void);
 *     - Returns 1 (DNS TYPE A soft catalog value). Pure-data RR type
 *       id stub; does not issue queries or call resolv/libc.
 *   uint32_t __gj_dns_a_id_8871  (alias)
 *   __libcgj_batch8871_marker = "libcgj-batch8871"
 *
 * Exclusive continuum CREATE-ONLY (8871-8880: resolv/dns id stubs —
 * dns_a_id_8871, dns_aaaa_id_8872, dns_cname_id_8873, dns_mx_id_8874,
 * dns_txt_id_8875, dns_ns_id_8876, dns_ptr_id_8877, dns_port_id_8878,
 * resolv_ready_u_8879, batch_id_8880).
 * Unique surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8871_marker[] = "libcgj-batch8871";

/* Soft DNS TYPE A catalog value (RR type 1). */
#define B8871_DNS_A  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8871_a_id(void)
{
	return B8871_DNS_A;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dns_a_id_8871 - soft DNS TYPE A RR type catalog id.
 *
 * Always returns 1. Soft compile-time RR type id; does not touch
 * resolv or sockets. No parent wires.
 */
uint32_t
gj_dns_a_id_8871(void)
{
	(void)NULL;
	return b8871_a_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dns_a_id_8871(void)
    __attribute__((alias("gj_dns_a_id_8871")));
