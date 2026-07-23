/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9927: Hyper-V VSS soft lamp stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hv_vss_ok_u_9927(void);
 *     - Return 0 (soft lamp: VSS not claimed present for this freestanding
 *       continuum). Catalog only; not a volume-shadow-copy probe.
 *   uint32_t __gj_hv_vss_ok_u_9927  (alias)
 *   __libcgj_batch9927_marker = "libcgj-batch9927"
 *
 * Exclusive continuum CREATE-ONLY (9921-9930: hyperv soft id stubs —
 * hv_vmbus_ok_u, hv_netvsc_ok_u, hv_storvsc_ok_u, hv_utils_ok_u,
 * hv_balloon_ok_u, hv_kvp_ok_u, hv_vss_ok_u, hv_fcopy_ok_u,
 * hyperv_ready_u, batch_id_9930). Unique gj_hv_vss_ok_u_9927 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9927_marker[] = "libcgj-batch9927";

/* Soft Hyper-V VSS lamp (not claimed / soft-off). */
#define B9927_HV_VSS_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9927_vss_ok(void)
{
	return B9927_HV_VSS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hv_vss_ok_u_9927 - soft Hyper-V VSS lamp.
 *
 * Always returns 0. Soft pure-data status tag; does not freeze
 * filesystems or handle volume shadow copy requests. No parent wires.
 */
uint32_t
gj_hv_vss_ok_u_9927(void)
{
	(void)NULL;
	return b9927_vss_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hv_vss_ok_u_9927(void)
    __attribute__((alias("gj_hv_vss_ok_u_9927")));
