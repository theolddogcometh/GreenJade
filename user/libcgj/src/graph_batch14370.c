/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14370: net/connectivity soft wave batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14370(void);
 *     - Returns the compile-time graph batch number for this TU (14370).
 *   uint32_t __gj_batch_id_14370  (alias)
 *   __libcgj_batch14370_marker = "libcgj-batch14370"
 *
 * Net/connectivity soft product CREATE-ONLY (14361-14370). Unique
 * gj_batch_id_14370 surface only; no multi-def. Distinct from
 * gj_batch_id_13870 (batch13870), gj_batch_id_13670 (batch13670),
 * gj_batch_id_13470 (batch13470), gj_batch_id_13270 (batch13270),
 * gj_net_ok_u_14361, gj_wifi_ok_u_14362, gj_ethernet_ok_u_14363,
 * gj_dns_ok_u_14364, gj_https_ok_u_14365, gj_cdn_ok_u_14366,
 * gj_steam_net_ok_u_14367, gj_offline_mode_ok_u_14368,
 * gj_net_soft_ready_u_14369, and prior gj_batch_id / gj_batch_id_N
 * symbols. Closes the 14361-14370 continuum. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14370_marker[] = "libcgj-batch14370";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14370_id(void)
{
	return 14370u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14370 - report this TU's graph batch number.
 *
 * Always returns 14370 (net/connectivity soft product wave id).
 * Link-time presence of this symbol tags the wave. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_14370(void)
{
	(void)NULL;
	return b14370_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14370(void)
    __attribute__((alias("gj_batch_id_14370")));
