/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13268: offline-mode soft product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_offline_mode_ok_u_13268(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       offline-mode capability probe for the net soft continuum.
 *   uint32_t __gj_offline_mode_ok_u_13268  (alias)
 *   __libcgj_batch13268_marker = "libcgj-batch13268"
 *
 * Exclusive continuum CREATE-ONLY (13261-13270: net soft id stubs —
 * ok slots all→0; net_soft_ready→1; batch_id→13270 —
 * net_ok_u_13261, wifi_ok_u_13262, ethernet_ok_u_13263,
 * dns_ok_u_13264, https_ok_u_13265, cdn_ok_u_13266,
 * steam_net_ok_u_13267, offline_mode_ok_u_13268,
 * net_soft_ready_u_13269, batch_id_13270). Unique surface only; no
 * multi-def. Distinct from gj_offline_mode_ok_u_13068 (batch13068),
 * gj_offline_mode_ok_u_12868 (batch12868), gj_offline_mode_ok_u_12668
 * (batch12668), gj_offline_mode_ok_u_12468 (batch12468), and sibling
 * net soft ok_u stubs in this wave. No parent wires. No __int128. No
 * offline-mode implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13268_marker[] = "libcgj-batch13268";

/* Soft offline-mode-ok lamp: always off (not a real offline probe). */
#define B13268_OFFLINE_MODE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13268_offline_mode_ok(void)
{
	return B13268_OFFLINE_MODE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_offline_mode_ok_u_13268 - offline-mode soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect offline state
 * or call libc. No parent wires.
 */
uint32_t
gj_offline_mode_ok_u_13268(void)
{
	(void)NULL;
	return b13268_offline_mode_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_offline_mode_ok_u_13268(void)
    __attribute__((alias("gj_offline_mode_ok_u_13268")));
