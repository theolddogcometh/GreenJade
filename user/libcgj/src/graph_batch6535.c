/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6535: NVMe queue depth soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_nvme_qdepth_ok_6535(uint32_t qdepth);
 *     - Soft queue depth gate: ok when depth in [2, 65536] inclusive.
 *       Outside range → 0, else 1. Soft stand-in for SQES/CQES size.
 *   uint32_t __gj_nvme_qdepth_ok_6535  (alias)
 *   __libcgj_batch6535_marker = "libcgj-batch6535"
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

const char __libcgj_batch6535_marker[] = "libcgj-batch6535";

#define B6535_QD_MIN      2u
#define B6535_QD_MAX  65536u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6535_qdepth_ok(uint32_t u32Depth)
{
	if (u32Depth < B6535_QD_MIN || u32Depth > B6535_QD_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nvme_qdepth_ok_6535 - soft NVMe queue depth validity.
 *
 * qdepth: desired queue entries (count, not 0-based AQA field)
 *
 * Returns 1 when depth is in [2, 65536], else 0. Pure integer bounds.
 * No parent wires.
 */
uint32_t
gj_nvme_qdepth_ok_6535(uint32_t qdepth)
{
	(void)NULL;
	return b6535_qdepth_ok(qdepth);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nvme_qdepth_ok_6535(uint32_t qdepth)
    __attribute__((alias("gj_nvme_qdepth_ok_6535")));
