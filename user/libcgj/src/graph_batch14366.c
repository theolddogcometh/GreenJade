/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14366: CDN connectivity soft product stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cdn_ok_u_14366(void);
 *     - Returns 0 (soft cdn-ok product lamp unset). Soft compile-time
 *       product tag for content-delivery network path readiness; not a
 *       runtime probe of CDN hosts.
 *   uint32_t __gj_cdn_ok_u_14366  (alias)
 *   __libcgj_batch14366_marker = "libcgj-batch14366"
 *
 * Net/connectivity soft product CREATE-ONLY (14361-14370). Unique
 * gj_cdn_ok_u_14366 surface only; no multi-def. Distinct from
 * gj_cdn_ok_u_13866 (batch13866), gj_cdn_ok_u_13666 (batch13666),
 * gj_cdn_ok_u_13466 (batch13466), gj_cdn_ok_u_13266 (batch13266),
 * gj_net_ok_u_14361, gj_wifi_ok_u_14362, gj_ethernet_ok_u_14363,
 * gj_dns_ok_u_14364, gj_https_ok_u_14365, gj_steam_net_ok_u_14367,
 * gj_offline_mode_ok_u_14368, gj_net_soft_ready_u_14369, and
 * gj_batch_id_14370. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14366_marker[] = "libcgj-batch14366";

/* Soft cdn-ok product lamp (unset / not probed). */
#define B14366_CDN_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14366_cdn_ok(void)
{
	return B14366_CDN_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cdn_ok_u_14366 - report soft CDN connectivity ok lamp.
 *
 * Always returns 0 (soft product stub unset). Link-time presence of
 * this symbol tags the CDN soft surface. Does not call libc or probe
 * CDN hosts. No parent wires.
 */
uint32_t
gj_cdn_ok_u_14366(void)
{
	(void)NULL;
	return b14366_cdn_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cdn_ok_u_14366(void)
    __attribute__((alias("gj_cdn_ok_u_14366")));
