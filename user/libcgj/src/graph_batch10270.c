/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10270: net/connectivity soft wave batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10270(void);
 *     - Returns the compile-time graph batch number for this TU (10270).
 *   uint32_t __gj_batch_id_10270  (alias)
 *   __libcgj_batch10270_marker = "libcgj-batch10270"
 *
 * Net/connectivity soft product CREATE-ONLY (10261-10270). Unique
 * gj_batch_id_10270 surface only; no multi-def. Distinct from
 * gj_net_ok_u_10261, gj_wifi_ok_u_10262, gj_ethernet_ok_u_10263,
 * gj_dns_ok_u_10264, gj_https_ok_u_10265, gj_cdn_ok_u_10266,
 * gj_steam_net_ok_u_10267, gj_offline_mode_ok_u_10268,
 * gj_net_soft_ready_u_10269, and prior gj_batch_id / gj_batch_id_N
 * symbols. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10270_marker[] = "libcgj-batch10270";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10270_id(void)
{
	return 10270u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10270 - report this TU's graph batch number.
 *
 * Always returns 10270 (net/connectivity soft product wave id).
 * Link-time presence of this symbol tags the wave. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_10270(void)
{
	(void)NULL;
	return b10270_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10270(void)
    __attribute__((alias("gj_batch_id_10270")));
