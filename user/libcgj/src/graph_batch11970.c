/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11970: net/connectivity soft wave batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11970(void);
 *     - Returns the compile-time graph batch number for this TU (11970).
 *   uint32_t __gj_batch_id_11970  (alias)
 *   __libcgj_batch11970_marker = "libcgj-batch11970"
 *
 * Net/connectivity soft product CREATE-ONLY (11961-11970). Unique
 * gj_batch_id_11970 surface only; no multi-def. Distinct from
 * gj_batch_id_11770 (batch11770), gj_batch_id_11570 (batch11570),
 * gj_net_ok_u_11961, gj_wifi_ok_u_11962, gj_ethernet_ok_u_11963,
 * gj_dns_ok_u_11964, gj_https_ok_u_11965, gj_cdn_ok_u_11966,
 * gj_steam_net_ok_u_11967, gj_offline_mode_ok_u_11968,
 * gj_net_soft_ready_u_11969, and prior gj_batch_id / gj_batch_id_N
 * symbols. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11970_marker[] = "libcgj-batch11970";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11970_id(void)
{
	return 11970u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11970 - report this TU's graph batch number.
 *
 * Always returns 11970 (net/connectivity soft product wave id).
 * Link-time presence of this symbol tags the wave. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_11970(void)
{
	(void)NULL;
	return b11970_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_11970(void)
    __attribute__((alias("gj_batch_id_11970")));
