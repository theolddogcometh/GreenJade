/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13270: net soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_13270(void);
 *     - Returns the compile-time graph batch number for this TU (13270).
 *   uint32_t __gj_batch_id_13270  (alias)
 *   __libcgj_batch13270_marker = "libcgj-batch13270"
 *
 * Exclusive continuum CREATE-ONLY (13261-13270: net soft id stubs —
 * ok slots all→0; net_soft_ready→1; batch_id→13270 —
 * net_ok_u_13261, wifi_ok_u_13262, ethernet_ok_u_13263,
 * dns_ok_u_13264, https_ok_u_13265, cdn_ok_u_13266,
 * steam_net_ok_u_13267, offline_mode_ok_u_13268,
 * net_soft_ready_u_13269, batch_id_13270). Unique surfaces only; no
 * multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. Distinct from
 * gj_batch_id_13070 (batch13070), gj_batch_id_12870 (batch12870),
 * gj_batch_id_12670 (batch12670). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13270_marker[] = "libcgj-batch13270";

#define B13270_BATCH_ID  13270u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13270_id(void)
{
	return B13270_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_13270 - report this TU's graph batch number.
 *
 * Always returns 13270.
 */
uint32_t
gj_batch_id_13270(void)
{
	(void)NULL;
	return b13270_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_13270(void)
    __attribute__((alias("gj_batch_id_13270")));
