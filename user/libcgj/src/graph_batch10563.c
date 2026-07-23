/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10563: ethernet ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_ethernet_ok_u_10563(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       ethernet link/carrier runtime probe for the net soft path.
 *   uint32_t __gj_ethernet_ok_u_10563  (alias)
 *   __libcgj_batch10563_marker = "libcgj-batch10563"
 *
 * Exclusive continuum CREATE-ONLY (10561-10570: net soft id stubs —
 * net_ok_u_10561, wifi_ok_u_10562, ethernet_ok_u_10563, dns_ok_u_10564,
 * https_ok_u_10565, cdn_ok_u_10566, steam_net_ok_u_10567,
 * offline_mode_ok_u_10568, net_soft_ready_u_10569, batch_id_10570).
 * Unique surface only; no multi-def. Distinct from
 * gj_ethernet_ok_u_10062. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10563_marker[] = "libcgj-batch10563";

/* Soft ethernet-ok lamp: always off (not a real runtime probe). */
#define B10563_ETHERNET_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10563_ok(void)
{
	return B10563_ETHERNET_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ethernet_ok_u_10563 - ethernet ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe ethernet link
 * or carrier or call libc. No parent wires.
 */
uint32_t
gj_ethernet_ok_u_10563(void)
{
	(void)NULL;
	return b10563_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ethernet_ok_u_10563(void)
    __attribute__((alias("gj_ethernet_ok_u_10563")));
