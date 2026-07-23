/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11370: net/connectivity soft wave batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11370(void);
 *     - Returns the compile-time graph batch number for this TU (11370).
 *   uint32_t __gj_batch_id_11370  (alias)
 *   __libcgj_batch11370_marker = "libcgj-batch11370"
 *
 * Net/connectivity soft product CREATE-ONLY (11361-11370). Unique
 * gj_batch_id_11370 surface only; no multi-def. Distinct from
 * gj_batch_id_11170 (batch11170), gj_batch_id_10970 (batch10970),
 * gj_net_ok_u_11361, gj_wifi_ok_u_11362, gj_ethernet_ok_u_11363,
 * gj_dns_ok_u_11364, gj_https_ok_u_11365, gj_cdn_ok_u_11366,
 * gj_steam_net_ok_u_11367, gj_offline_mode_ok_u_11368,
 * gj_net_soft_ready_u_11369, and prior gj_batch_id / gj_batch_id_N
 * symbols. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11370_marker[] = "libcgj-batch11370";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11370_id(void)
{
	return 11370u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11370 - report this TU's graph batch number.
 *
 * Always returns 11370 (net/connectivity soft product wave id).
 * Link-time presence of this symbol tags the wave. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_11370(void)
{
	(void)NULL;
	return b11370_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_11370(void)
    __attribute__((alias("gj_batch_id_11370")));
