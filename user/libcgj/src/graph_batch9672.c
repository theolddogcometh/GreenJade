/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9672: soft WireGuard peer-ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wg_peer_ok_u_9672(void);
 *     - Returns 1 (wg peer soft-id continuum ok). Pure-data product
 *       tag; does not list peers or call WireGuard.
 *   uint32_t __gj_wg_peer_ok_u_9672  (alias)
 *   __libcgj_batch9672_marker = "libcgj-batch9672"
 *
 * Exclusive continuum CREATE-ONLY (9671-9680: wireguard soft id stubs —
 * wg_device_ok_u_9671, wg_peer_ok_u_9672, wg_key_ok_u_9673,
 * wg_endpoint_ok_u_9674, wg_allowed_ips_ok_u_9675, wg_up_ok_u_9676,
 * wg_down_ok_u_9677, wg_handshake_ok_u_9678, wireguard_ready_u_9679,
 * batch_id_9680). Unique surface only; no multi-def. Distinct from
 * device/key ok flags. No parent wires. No __int128. No WireGuard
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9672_marker[] = "libcgj-batch9672";

#define B9672_WG_PEER_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9672_peer_ok(void)
{
	return B9672_WG_PEER_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wg_peer_ok_u_9672 - WireGuard peer soft-id continuum ok flag.
 *
 * Always returns 1. Soft pure-data product tag; does not list peers
 * or call WireGuard. No parent wires.
 */
uint32_t
gj_wg_peer_ok_u_9672(void)
{
	(void)NULL;
	return b9672_peer_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wg_peer_ok_u_9672(void)
    __attribute__((alias("gj_wg_peer_ok_u_9672")));
