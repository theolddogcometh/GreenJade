/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9924: Hyper-V utils soft lamp stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hv_utils_ok_u_9924(void);
 *     - Return 1 (soft lamp: hv_utils soft-id continuum ok). Catalog only;
 *       not a live util services probe.
 *   uint32_t __gj_hv_utils_ok_u_9924  (alias)
 *   __libcgj_batch9924_marker = "libcgj-batch9924"
 *
 * Exclusive continuum CREATE-ONLY (9921-9930: hyperv soft id stubs —
 * hv_vmbus_ok_u, hv_netvsc_ok_u, hv_storvsc_ok_u, hv_utils_ok_u,
 * hv_balloon_ok_u, hv_kvp_ok_u, hv_vss_ok_u, hv_fcopy_ok_u,
 * hyperv_ready_u, batch_id_9930). Unique gj_hv_utils_ok_u_9924 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9924_marker[] = "libcgj-batch9924";

/* Soft Hyper-V utils lamp (soft-id continuum ok). */
#define B9924_HV_UTILS_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9924_utils_ok(void)
{
	return B9924_HV_UTILS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hv_utils_ok_u_9924 - soft Hyper-V utils lamp.
 *
 * Always returns 1. Soft pure-data status tag; does not register
 * shutdown/heartbeat util services. No parent wires.
 */
uint32_t
gj_hv_utils_ok_u_9924(void)
{
	(void)NULL;
	return b9924_utils_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hv_utils_ok_u_9924(void)
    __attribute__((alias("gj_hv_utils_ok_u_9924")));
