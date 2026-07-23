/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8860: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8860(void);
 *     - Returns the compile-time graph batch number for this TU (8860).
 *   uint32_t __gj_batch_id_8860  (alias)
 *   __libcgj_batch8860_marker = "libcgj-batch8860"
 *
 * Exclusive continuum CREATE-ONLY (8851-8860: if_nametoindex stubs —
 * if_name_ok_u, if_index_ok_u, if_loop_id, if_eth_ok_u, if_up_id,
 * if_running_id, if_mtu_ok_u, if_mac_len_id, if_ready_u,
 * batch_id_8860). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. Soft pure-data catalog
 * only; no if_nametoindex. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8860_marker[] = "libcgj-batch8860";

#define B8860_BATCH_ID  8860u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8860_id(void)
{
	return B8860_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8860 - report this TU's graph batch number.
 *
 * Always returns 8860. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_8860(void)
{
	(void)NULL;
	return b8860_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8860(void)
    __attribute__((alias("gj_batch_id_8860")));
