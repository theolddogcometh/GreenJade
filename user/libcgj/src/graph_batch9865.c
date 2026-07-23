/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9865: soft NVMe I/O-ok unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_nvme_io_ok_u_9865(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       NVMe I/O queue / read-write completion status probe.
 *   uint32_t __gj_nvme_io_ok_u_9865  (alias)
 *   __libcgj_batch9865_marker = "libcgj-batch9865"
 *
 * Exclusive continuum CREATE-ONLY (9861-9870: nvme soft id stubs —
 * nvme_ctrl_ok_u_9861, nvme_ns_ok_u_9862, nvme_queue_ok_u_9863,
 * nvme_admin_ok_u_9864, nvme_io_ok_u_9865, nvme_identify_ok_u_9866,
 * nvme_format_ok_u_9867, nvme_fw_ok_u_9868, nvme_ready_u_9869,
 * batch_id_9870). Unique surface only; no multi-def. Distinct from
 * admin ok flag. No parent wires. No __int128. No NVMe driver /
 * hardware implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9865_marker[] = "libcgj-batch9865";

/* Soft I/O-ok lamp: always off (not a real I/O completion probe). */
#define B9865_NVME_IO_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9865_io_ok(void)
{
	return B9865_NVME_IO_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nvme_io_ok_u_9865 - NVMe I/O soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not submit I/O commands
 * or talk to hardware. No parent wires.
 */
uint32_t
gj_nvme_io_ok_u_9865(void)
{
	(void)NULL;
	return b9865_io_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nvme_io_ok_u_9865(void)
    __attribute__((alias("gj_nvme_io_ok_u_9865")));
