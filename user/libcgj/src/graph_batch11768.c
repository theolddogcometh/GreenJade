/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11768: offline-mode soft product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_offline_mode_ok_u_11768(void);
 *     - Returns 0 (soft offline-mode-ok product lamp unset). Soft
 *       compile-time product tag for offline/local-only mode readiness;
 *       not a runtime probe of network or package state.
 *   uint32_t __gj_offline_mode_ok_u_11768  (alias)
 *   __libcgj_batch11768_marker = "libcgj-batch11768"
 *
 * Net/connectivity soft product CREATE-ONLY (11761-11770). Unique
 * gj_offline_mode_ok_u_11768 surface only; no multi-def. Distinct from
 * gj_offline_mode_ok_u_11568 (batch11568), gj_offline_mode_ok_u_11368
 * (batch11368), gj_net_ok_u_11761, gj_wifi_ok_u_11762,
 * gj_ethernet_ok_u_11763, gj_dns_ok_u_11764, gj_https_ok_u_11765,
 * gj_cdn_ok_u_11766, gj_steam_net_ok_u_11767,
 * gj_net_soft_ready_u_11769, and gj_batch_id_11770. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11768_marker[] = "libcgj-batch11768";

/* Soft offline-mode-ok product lamp (unset / not probed). */
#define B11768_OFFLINE_MODE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11768_offline_mode_ok(void)
{
	return B11768_OFFLINE_MODE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_offline_mode_ok_u_11768 - report soft offline-mode ok lamp.
 *
 * Always returns 0 (soft product stub unset). Link-time presence of
 * this symbol tags the offline-mode soft surface. Does not call libc
 * or probe network state. No parent wires.
 */
uint32_t
gj_offline_mode_ok_u_11768(void)
{
	(void)NULL;
	return b11768_offline_mode_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_offline_mode_ok_u_11768(void)
    __attribute__((alias("gj_offline_mode_ok_u_11768")));
