/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6539: NVMe I/O queue count soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_nvme_io_queues_ok_6539(uint32_t nsq, uint32_t ncq);
 *     - Soft I/O queue pair count gate: both nsq and ncq in [1, 65535]
 *       and ncq >= nsq → 1; else 0. Soft stand-in for Set Features #7.
 *   uint32_t __gj_nvme_io_queues_ok_6539  (alias)
 *   __libcgj_batch6539_marker = "libcgj-batch6539"
 *
 * Exclusive continuum CREATE-ONLY (6531-6540: NVMe namespace/queue depth
 * stubs — nsid_ok_6531, ns_active_p_6532, lba_count_ok_6533,
 * mdts_clamp_6534, qdepth_ok_6535, sq_size_pack_6536, cq_size_pack_6537,
 * qid_ok_6538, io_queues_ok_6539, ns_qd_ready_6540). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6539_marker[] = "libcgj-batch6539";

#define B6539_Q_MIN      1u
#define B6539_Q_MAX  65535u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6539_io_queues_ok(uint32_t u32Nsq, uint32_t u32Ncq)
{
	if (u32Nsq < B6539_Q_MIN || u32Nsq > B6539_Q_MAX) {
		return 0u;
	}
	if (u32Ncq < B6539_Q_MIN || u32Ncq > B6539_Q_MAX) {
		return 0u;
	}
	if (u32Ncq < u32Nsq) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nvme_io_queues_ok_6539 - soft NVMe I/O queue count pair gate.
 *
 * nsq: number of I/O submission queues requested
 * ncq: number of I/O completion queues requested
 *
 * Returns 1 when both in [1, 65535] and ncq >= nsq, else 0.
 * Pure integer. No parent wires.
 */
uint32_t
gj_nvme_io_queues_ok_6539(uint32_t nsq, uint32_t ncq)
{
	(void)NULL;
	return b6539_io_queues_ok(nsq, ncq);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nvme_io_queues_ok_6539(uint32_t nsq, uint32_t ncq)
    __attribute__((alias("gj_nvme_io_queues_ok_6539")));
