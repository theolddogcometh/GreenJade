/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12670: net/connectivity soft wave batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12670(void);
 *     - Returns the compile-time graph batch number for this TU (12670).
 *   uint32_t __gj_batch_id_12670  (alias)
 *   __libcgj_batch12670_marker = "libcgj-batch12670"
 *
 * Net/connectivity soft product CREATE-ONLY (12661-12670). Unique
 * gj_batch_id_12670 surface only; no multi-def. Distinct from
 * gj_batch_id_12470 (batch12470), gj_batch_id_12270 (batch12270),
 * gj_net_ok_u_12661, gj_wifi_ok_u_12662, gj_ethernet_ok_u_12663,
 * gj_dns_ok_u_12664, gj_https_ok_u_12665, gj_cdn_ok_u_12666,
 * gj_steam_net_ok_u_12667, gj_offline_mode_ok_u_12668,
 * gj_net_soft_ready_u_12669, and prior gj_batch_id / gj_batch_id_N
 * symbols. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12670_marker[] = "libcgj-batch12670";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12670_id(void)
{
	return 12670u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12670 - report this TU's graph batch number.
 *
 * Always returns 12670 (net/connectivity soft product wave id).
 * Link-time presence of this symbol tags the wave. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_12670(void)
{
	(void)NULL;
	return b12670_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12670(void)
    __attribute__((alias("gj_batch_id_12670")));
