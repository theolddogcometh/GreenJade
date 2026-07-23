/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12470: net/connectivity soft wave batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12470(void);
 *     - Returns the compile-time graph batch number for this TU (12470).
 *   uint32_t __gj_batch_id_12470  (alias)
 *   __libcgj_batch12470_marker = "libcgj-batch12470"
 *
 * Net/connectivity soft product CREATE-ONLY (12461-12470). Unique
 * gj_batch_id_12470 surface only; no multi-def. Distinct from
 * gj_batch_id_12070 (batch12070), gj_batch_id_11970 (batch11970),
 * gj_batch_id_11770 (batch11770), gj_net_ok_u_12461,
 * gj_wifi_ok_u_12462, gj_ethernet_ok_u_12463, gj_dns_ok_u_12464,
 * gj_https_ok_u_12465, gj_cdn_ok_u_12466, gj_steam_net_ok_u_12467,
 * gj_offline_mode_ok_u_12468, gj_net_soft_ready_u_12469, and prior
 * gj_batch_id / gj_batch_id_N symbols. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12470_marker[] = "libcgj-batch12470";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12470_id(void)
{
	return 12470u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12470 - report this TU's graph batch number.
 *
 * Always returns 12470 (net/connectivity soft product wave id).
 * Link-time presence of this symbol tags the wave. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_batch_id_12470(void)
{
	(void)NULL;
	return b12470_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12470(void)
    __attribute__((alias("gj_batch_id_12470")));
