/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13668: offline-mode soft product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_offline_mode_ok_u_13668(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       offline-mode capability probe for the net soft continuum.
 *   uint32_t __gj_offline_mode_ok_u_13668  (alias)
 *   __libcgj_batch13668_marker = "libcgj-batch13668"
 *
 * Exclusive continuum CREATE-ONLY (13661-13670: net soft id stubs —
 * ok slots all→0; net_soft_ready→1; batch_id→13670 —
 * net_ok_u_13661, wifi_ok_u_13662, ethernet_ok_u_13663,
 * dns_ok_u_13664, https_ok_u_13665, cdn_ok_u_13666,
 * steam_net_ok_u_13667, offline_mode_ok_u_13668,
 * net_soft_ready_u_13669, batch_id_13670). Unique surface only; no
 * multi-def. Distinct from gj_offline_mode_ok_u_13468 (batch13468),
 * gj_offline_mode_ok_u_13268 (batch13268), gj_offline_mode_ok_u_13068
 * (batch13068), gj_offline_mode_ok_u_12868 (batch12868), and sibling
 * net soft ok_u stubs in this wave. No parent wires. No __int128. No
 * offline-mode implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13668_marker[] = "libcgj-batch13668";

/* Soft offline-mode-ok lamp: always off (not a real offline probe). */
#define B13668_OFFLINE_MODE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13668_offline_mode_ok(void)
{
	return B13668_OFFLINE_MODE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_offline_mode_ok_u_13668 - offline-mode soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect offline
 * capability or call libc. No parent wires.
 */
uint32_t
gj_offline_mode_ok_u_13668(void)
{
	(void)NULL;
	return b13668_offline_mode_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_offline_mode_ok_u_13668(void)
    __attribute__((alias("gj_offline_mode_ok_u_13668")));
