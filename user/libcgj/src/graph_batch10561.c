/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10561: net ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_net_ok_u_10561(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       network connectivity runtime probe for the net soft path.
 *   uint32_t __gj_net_ok_u_10561  (alias)
 *   __libcgj_batch10561_marker = "libcgj-batch10561"
 *
 * Exclusive continuum CREATE-ONLY (10561-10570: net soft id stubs —
 * net_ok_u_10561, wifi_ok_u_10562, ethernet_ok_u_10563, dns_ok_u_10564,
 * https_ok_u_10565, cdn_ok_u_10566, steam_net_ok_u_10567,
 * offline_mode_ok_u_10568, net_soft_ready_u_10569, batch_id_10570).
 * Unique surface only; no multi-def. Distinct from gj_wifi_ok_u_10061
 * wave (10061-10070) and netmgr wave (10471-10480). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10561_marker[] = "libcgj-batch10561";

/* Soft net-ok lamp: always off (not a real runtime probe). */
#define B10561_NET_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10561_ok(void)
{
	return B10561_NET_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_net_ok_u_10561 - net ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe network
 * connectivity or call libc. No parent wires.
 */
uint32_t
gj_net_ok_u_10561(void)
{
	(void)NULL;
	return b10561_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_net_ok_u_10561(void)
    __attribute__((alias("gj_net_ok_u_10561")));
