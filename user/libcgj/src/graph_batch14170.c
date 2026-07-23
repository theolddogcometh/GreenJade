/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14170: net soft deepen wave batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14170(void);
 *     - Returns the compile-time graph batch number for this TU (14170).
 *   uint32_t __gj_batch_id_14170  (alias)
 *   __libcgj_batch14170_marker = "libcgj-batch14170"
 *
 * Exclusive continuum CREATE-ONLY net soft deepen (14161-14170). Unique
 * gj_batch_id_14170 surface only; no multi-def. Distinct from
 * gj_batch_id_13870 (batch13870), gj_net_ok_u_14161, gj_wifi_ok_u_14162,
 * gj_ethernet_ok_u_14163, gj_dns_ok_u_14164, gj_https_ok_u_14165,
 * gj_cdn_ok_u_14166, gj_steam_net_ok_u_14167, gj_offline_mode_ok_u_14168,
 * gj_net_soft_ready_u_14169, and prior gj_batch_id / gj_batch_id_N
 * symbols. Closes the 14161-14170 continuum. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14170_marker[] = "libcgj-batch14170";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14170_id(void)
{
	return 14170u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14170 - report this TU's graph batch number.
 *
 * Always returns 14170 (net soft deepen product wave id). Link-time
 * presence of this symbol tags the wave. Does not call libc. No
 * parent wires.
 */
uint32_t
gj_batch_id_14170(void)
{
	(void)NULL;
	return b14170_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_14170(void)
    __attribute__((alias("gj_batch_id_14170")));
