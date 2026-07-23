/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6538: NVMe queue id soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_nvme_qid_ok_6538(uint32_t qid, uint32_t max_qid);
 *     - Soft QID gate: qid == 0 (admin) always ok; I/O qid in
 *       [1, max_qid] when max_qid non-zero. max_qid 0 rejects I/O qids.
 *   uint32_t __gj_nvme_qid_ok_6538  (alias)
 *   __libcgj_batch6538_marker = "libcgj-batch6538"
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

const char __libcgj_batch6538_marker[] = "libcgj-batch6538";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6538_qid_ok(uint32_t u32Qid, uint32_t u32Max)
{
	if (u32Qid == 0u) {
		return 1u;
	}
	if (u32Max == 0u || u32Qid > u32Max) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nvme_qid_ok_6538 - soft NVMe queue identifier validity.
 *
 * qid:     queue id (0 = admin queue)
 * max_qid: soft max I/O queue id (0 → only admin allowed)
 *
 * Returns 1 when qid is admissible, else 0. Pure integer. No parent wires.
 */
uint32_t
gj_nvme_qid_ok_6538(uint32_t qid, uint32_t max_qid)
{
	(void)NULL;
	return b6538_qid_ok(qid, max_qid);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nvme_qid_ok_6538(uint32_t qid, uint32_t max_qid)
    __attribute__((alias("gj_nvme_qid_ok_6538")));
