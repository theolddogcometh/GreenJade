/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10070: network product soft batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10070(void);
 *     - Returns the compile-time graph batch number for this TU (10070).
 *   uint32_t __gj_batch_id_10070  (alias)
 *   __libcgj_batch10070_marker = "libcgj-batch10070"
 *
 * Exclusive network product soft id CREATE-ONLY (10061-10070:
 * wifi_ok, ethernet_ok, dns_ok, dhcp_ok, ntp_ok, https_ok, cdn_ok,
 * steam_cdn_ok, net_soft_ready, batch_id_10070). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols - avoid
 * multi-def. Soft pure-data catalog only. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10070_marker[] = "libcgj-batch10070";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10070_id(void)
{
	return 10070u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10070 - report this TU's graph batch number.
 *
 * Always returns 10070. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_10070(void)
{
	(void)NULL;
	return b10070_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10070(void)
    __attribute__((alias("gj_batch_id_10070")));
