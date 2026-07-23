/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11770: net/connectivity soft wave batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11770(void);
 *     - Returns the compile-time graph batch number for this TU (11770).
 *   uint32_t __gj_batch_id_11770  (alias)
 *   __libcgj_batch11770_marker = "libcgj-batch11770"
 *
 * Net/connectivity soft product CREATE-ONLY (11761-11770). Unique
 * gj_batch_id_11770 surface only; no multi-def. Distinct from
 * gj_batch_id_11570 (batch11570), gj_batch_id_11370 (batch11370),
 * gj_net_ok_u_11761, gj_wifi_ok_u_11762, gj_ethernet_ok_u_11763,
 * gj_dns_ok_u_11764, gj_https_ok_u_11765, gj_cdn_ok_u_11766,
 * gj_steam_net_ok_u_11767, gj_offline_mode_ok_u_11768,
 * gj_net_soft_ready_u_11769, and prior gj_batch_id / gj_batch_id_N
 * symbols. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11770_marker[] = "libcgj-batch11770";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11770_id(void)
{
	return 11770u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11770 - report this TU's graph batch number.
 *
 * Always returns 11770 (net/connectivity soft product wave id).
 * Link-time presence of this symbol tags the wave. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_11770(void)
{
	(void)NULL;
	return b11770_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_11770(void)
    __attribute__((alias("gj_batch_id_11770")));
