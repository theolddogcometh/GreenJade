/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9862: soft NVMe namespace-ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_nvme_ns_ok_u_9862(void);
 *     - Returns 1 (nvme namespace soft-id continuum ok). Pure-data
 *       product tag; does not enumerate namespaces or talk to hardware.
 *   uint32_t __gj_nvme_ns_ok_u_9862  (alias)
 *   __libcgj_batch9862_marker = "libcgj-batch9862"
 *
 * Exclusive continuum CREATE-ONLY (9861-9870: nvme soft id stubs —
 * nvme_ctrl_ok_u_9861, nvme_ns_ok_u_9862, nvme_queue_ok_u_9863,
 * nvme_admin_ok_u_9864, nvme_io_ok_u_9865, nvme_identify_ok_u_9866,
 * nvme_format_ok_u_9867, nvme_fw_ok_u_9868, nvme_ready_u_9869,
 * batch_id_9870). Unique surface only; no multi-def. Distinct from
 * ctrl ok flag. No parent wires. No __int128. No NVMe driver /
 * hardware implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9862_marker[] = "libcgj-batch9862";

#define B9862_NVME_NS_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9862_ns_ok(void)
{
	return B9862_NVME_NS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nvme_ns_ok_u_9862 - NVMe namespace soft-id continuum ok flag.
 *
 * Always returns 1. Soft pure-data product tag; does not enumerate
 * namespaces or talk to hardware. No parent wires.
 */
uint32_t
gj_nvme_ns_ok_u_9862(void)
{
	(void)NULL;
	return b9862_ns_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nvme_ns_ok_u_9862(void)
    __attribute__((alias("gj_nvme_ns_ok_u_9862")));
