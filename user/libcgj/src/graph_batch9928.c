/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9928: Hyper-V fcopy soft lamp stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hv_fcopy_ok_u_9928(void);
 *     - Return 0 (soft lamp: fcopy not claimed present for this freestanding
 *       continuum). Catalog only; not a host file-copy service probe.
 *   uint32_t __gj_hv_fcopy_ok_u_9928  (alias)
 *   __libcgj_batch9928_marker = "libcgj-batch9928"
 *
 * Exclusive continuum CREATE-ONLY (9921-9930: hyperv soft id stubs —
 * hv_vmbus_ok_u, hv_netvsc_ok_u, hv_storvsc_ok_u, hv_utils_ok_u,
 * hv_balloon_ok_u, hv_kvp_ok_u, hv_vss_ok_u, hv_fcopy_ok_u,
 * hyperv_ready_u, batch_id_9930). Unique gj_hv_fcopy_ok_u_9928 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9928_marker[] = "libcgj-batch9928";

/* Soft Hyper-V fcopy lamp (not claimed / soft-off). */
#define B9928_HV_FCOPY_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9928_fcopy_ok(void)
{
	return B9928_HV_FCOPY_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hv_fcopy_ok_u_9928 - soft Hyper-V fcopy lamp.
 *
 * Always returns 0. Soft pure-data status tag; does not receive host
 * file-copy transfers. No parent wires.
 */
uint32_t
gj_hv_fcopy_ok_u_9928(void)
{
	(void)NULL;
	return b9928_fcopy_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hv_fcopy_ok_u_9928(void)
    __attribute__((alias("gj_hv_fcopy_ok_u_9928")));
