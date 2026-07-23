/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9923: Hyper-V storvsc soft lamp stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hv_storvsc_ok_u_9923(void);
 *     - Return 1 (soft lamp: storvsc soft-id continuum ok). Catalog only;
 *       not a live SCSI VSC probe.
 *   uint32_t __gj_hv_storvsc_ok_u_9923  (alias)
 *   __libcgj_batch9923_marker = "libcgj-batch9923"
 *
 * Exclusive continuum CREATE-ONLY (9921-9930: hyperv soft id stubs —
 * hv_vmbus_ok_u, hv_netvsc_ok_u, hv_storvsc_ok_u, hv_utils_ok_u,
 * hv_balloon_ok_u, hv_kvp_ok_u, hv_vss_ok_u, hv_fcopy_ok_u,
 * hyperv_ready_u, batch_id_9930). Unique gj_hv_storvsc_ok_u_9923 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9923_marker[] = "libcgj-batch9923";

/* Soft Hyper-V storvsc lamp (soft-id continuum ok). */
#define B9923_HV_STORVSC_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9923_storvsc_ok(void)
{
	return B9923_HV_STORVSC_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hv_storvsc_ok_u_9923 - soft Hyper-V storvsc lamp.
 *
 * Always returns 1. Soft pure-data status tag; does not claim SCSI
 * LUNs or issue storage VSC I/O. No parent wires.
 */
uint32_t
gj_hv_storvsc_ok_u_9923(void)
{
	(void)NULL;
	return b9923_storvsc_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hv_storvsc_ok_u_9923(void)
    __attribute__((alias("gj_hv_storvsc_ok_u_9923")));
