/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13470: net soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13470(void);
 *     - Returns the compile-time graph batch number for this TU (13470).
 *   uint32_t __gj_batch_id_13470  (alias)
 *   __libcgj_batch13470_marker = "libcgj-batch13470"
 *
 * Exclusive continuum CREATE-ONLY (13461-13470: net soft id stubs —
 * ok slots all→0; net_soft_ready→1; batch_id→13470 —
 * net_ok_u_13461, wifi_ok_u_13462, ethernet_ok_u_13463,
 * dns_ok_u_13464, https_ok_u_13465, cdn_ok_u_13466,
 * steam_net_ok_u_13467, offline_mode_ok_u_13468,
 * net_soft_ready_u_13469, batch_id_13470). Unique surfaces only; no
 * multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_13270 (batch13270), gj_batch_id_13070 (batch13070),
 * gj_batch_id_12870 (batch12870). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13470_marker[] = "libcgj-batch13470";

#define B13470_BATCH_ID  13470u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13470_id(void)
{
	return B13470_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13470 - report this TU's graph batch number.
 *
 * Always returns 13470.
 */
uint32_t
gj_batch_id_13470(void)
{
	(void)NULL;
	return b13470_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13470(void)
    __attribute__((alias("gj_batch_id_13470")));
