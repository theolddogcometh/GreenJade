/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10564: dns ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_dns_ok_u_10564(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       DNS resolver/nameserver runtime probe for the net soft path.
 *   uint32_t __gj_dns_ok_u_10564  (alias)
 *   __libcgj_batch10564_marker = "libcgj-batch10564"
 *
 * Exclusive continuum CREATE-ONLY (10561-10570: net soft id stubs —
 * net_ok_u_10561, wifi_ok_u_10562, ethernet_ok_u_10563, dns_ok_u_10564,
 * https_ok_u_10565, cdn_ok_u_10566, steam_net_ok_u_10567,
 * offline_mode_ok_u_10568, net_soft_ready_u_10569, batch_id_10570).
 * Unique surface only; no multi-def. Distinct from gj_dns_ok_u_10063.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10564_marker[] = "libcgj-batch10564";

/* Soft dns-ok lamp: always off (not a real runtime probe). */
#define B10564_DNS_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10564_ok(void)
{
	return B10564_DNS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dns_ok_u_10564 - dns ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe DNS resolvers
 * or nameservers or call libc. No parent wires.
 */
uint32_t
gj_dns_ok_u_10564(void)
{
	(void)NULL;
	return b10564_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dns_ok_u_10564(void)
    __attribute__((alias("gj_dns_ok_u_10564")));
