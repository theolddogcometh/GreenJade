/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10370: net soft continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10370(void);
 *     - Returns the compile-time graph batch number for this TU (10370).
 *   uint32_t __gj_batch_id_10370  (alias)
 *   __libcgj_batch10370_marker = "libcgj-batch10370"
 *
 * Exclusive continuum CREATE-ONLY (10361-10370: net soft all→0 —
 * net_ok_u_10361, wifi_ok_u_10362, ethernet_ok_u_10363, dns_ok_u_10364,
 * https_ok_u_10365, cdn_ok_u_10366, steam_net_ok_u_10367,
 * offline_mode_ok_u_10368, net_soft_ready_u_10369, batch_id_10370).
 * Unique surface only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10370_marker[] = "libcgj-batch10370";

#define B10370_BATCH_ID  10370u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10370_id(void)
{
	return B10370_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10370 - report this TU's graph batch number.
 *
 * Always returns 10370. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_10370(void)
{
	(void)NULL;
	return b10370_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10370(void)
    __attribute__((alias("gj_batch_id_10370")));
