/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9864: soft NVMe admin-ok unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_nvme_admin_ok_u_9864(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       NVMe admin-queue / admin-command completion status probe.
 *   uint32_t __gj_nvme_admin_ok_u_9864  (alias)
 *   __libcgj_batch9864_marker = "libcgj-batch9864"
 *
 * Exclusive continuum CREATE-ONLY (9861-9870: nvme soft id stubs —
 * nvme_ctrl_ok_u_9861, nvme_ns_ok_u_9862, nvme_queue_ok_u_9863,
 * nvme_admin_ok_u_9864, nvme_io_ok_u_9865, nvme_identify_ok_u_9866,
 * nvme_format_ok_u_9867, nvme_fw_ok_u_9868, nvme_ready_u_9869,
 * batch_id_9870). Unique surface only; no multi-def. Distinct from
 * ctrl/ns/queue ok flags. No parent wires. No __int128. No NVMe driver
 * / hardware implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9864_marker[] = "libcgj-batch9864";

/* Soft admin-ok lamp: always off (not a real admin-command probe). */
#define B9864_NVME_ADMIN_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9864_admin_ok(void)
{
	return B9864_NVME_ADMIN_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nvme_admin_ok_u_9864 - NVMe admin soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not submit admin commands
 * or talk to hardware. No parent wires.
 */
uint32_t
gj_nvme_admin_ok_u_9864(void)
{
	(void)NULL;
	return b9864_admin_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nvme_admin_ok_u_9864(void)
    __attribute__((alias("gj_nvme_admin_ok_u_9864")));
