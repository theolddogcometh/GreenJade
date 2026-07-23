/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10363: network soft ethernet-ok lamp (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_ethernet_ok_u_10363(void);
 *     - Always returns 0. Soft stub only; intentionally not a live
 *       link/carrier probe for the net soft continuum.
 *   uint32_t __gj_ethernet_ok_u_10363  (alias)
 *   __libcgj_batch10363_marker = "libcgj-batch10363"
 *
 * Exclusive continuum CREATE-ONLY (10361-10370: net soft all→0 —
 * net_ok_u_10361, wifi_ok_u_10362, ethernet_ok_u_10363, dns_ok_u_10364,
 * https_ok_u_10365, cdn_ok_u_10366, steam_net_ok_u_10367,
 * offline_mode_ok_u_10368, net_soft_ready_u_10369, batch_id_10370).
 * Unique surface only; no multi-def. Distinct from
 * gj_ethernet_ok_u_10062. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10363_marker[] = "libcgj-batch10363";

/* Soft ethernet-ok lamp: always off (not a real carrier probe). */
#define B10363_ETHERNET_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10363_ethernet_ok(void)
{
	return B10363_ETHERNET_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ethernet_ok_u_10363 - network soft ethernet-ok unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe interfaces or
 * call libc. No parent wires.
 */
uint32_t
gj_ethernet_ok_u_10363(void)
{
	(void)NULL;
	return b10363_ethernet_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ethernet_ok_u_10363(void)
    __attribute__((alias("gj_ethernet_ok_u_10363")));
