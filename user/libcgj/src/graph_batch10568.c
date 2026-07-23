/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10568: offline mode ok soft unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_offline_mode_ok_u_10568(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       offline-mode policy runtime probe for the net soft path.
 *   uint32_t __gj_offline_mode_ok_u_10568  (alias)
 *   __libcgj_batch10568_marker = "libcgj-batch10568"
 *
 * Exclusive continuum CREATE-ONLY (10561-10570: net soft id stubs —
 * net_ok_u_10561, wifi_ok_u_10562, ethernet_ok_u_10563, dns_ok_u_10564,
 * https_ok_u_10565, cdn_ok_u_10566, steam_net_ok_u_10567,
 * offline_mode_ok_u_10568, net_soft_ready_u_10569, batch_id_10570).
 * Unique surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10568_marker[] = "libcgj-batch10568";

/* Soft offline-mode-ok lamp: always off (not a real runtime probe). */
#define B10568_OFFLINE_MODE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10568_ok(void)
{
	return B10568_OFFLINE_MODE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_offline_mode_ok_u_10568 - offline mode ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not probe offline-mode
 * policy or call libc. No parent wires.
 */
uint32_t
gj_offline_mode_ok_u_10568(void)
{
	(void)NULL;
	return b10568_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_offline_mode_ok_u_10568(void)
    __attribute__((alias("gj_offline_mode_ok_u_10568")));
