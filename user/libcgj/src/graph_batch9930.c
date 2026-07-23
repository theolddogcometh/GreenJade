/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9930: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9930(void);
 *     - Returns the compile-time graph batch number for this TU (9930).
 *   uint32_t __gj_batch_id_9930  (alias)
 *   __libcgj_batch9930_marker = "libcgj-batch9930"
 *
 * Exclusive continuum CREATE-ONLY (9921-9930: hyperv soft id stubs —
 * hv_vmbus_ok_u, hv_netvsc_ok_u, hv_storvsc_ok_u, hv_utils_ok_u,
 * hv_balloon_ok_u, hv_kvp_ok_u, hv_vss_ok_u, hv_fcopy_ok_u,
 * hyperv_ready_u, batch_id_9930). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * Soft pure-data catalog only; no Hyper-V I/O. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9930_marker[] = "libcgj-batch9930";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9930_id(void)
{
	return 9930u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9930 - report this TU's graph batch number.
 *
 * Always returns 9930. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_9930(void)
{
	(void)NULL;
	return b9930_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9930(void)
    __attribute__((alias("gj_batch_id_9930")));
