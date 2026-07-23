/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6533: NVMe namespace LBA count soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_nvme_lba_count_ok_6533(uint32_t nlb, uint32_t max_nlb);
 *     - Soft transfer LBA count gate: nlb in [1, max_nlb] when max_nlb
 *       non-zero; max_nlb 0 → reject. Pure soft bounds (not NLBA/NLB wire).
 *   uint32_t __gj_nvme_lba_count_ok_6533  (alias)
 *   __libcgj_batch6533_marker = "libcgj-batch6533"
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

const char __libcgj_batch6533_marker[] = "libcgj-batch6533";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6533_lba_count_ok(uint32_t u32Nlb, uint32_t u32Max)
{
	if (u32Max == 0u || u32Nlb == 0u) {
		return 0u;
	}
	if (u32Nlb > u32Max) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nvme_lba_count_ok_6533 - soft NVMe LBA transfer count gate.
 *
 * nlb:     requested logical-block count (soft, 1-based count)
 * max_nlb: soft ceiling from MDTS/NSZE fold (0 → reject)
 *
 * Returns 1 when nlb is in [1, max_nlb], else 0. Pure integer.
 * No parent wires.
 */
uint32_t
gj_nvme_lba_count_ok_6533(uint32_t nlb, uint32_t max_nlb)
{
	(void)NULL;
	return b6533_lba_count_ok(nlb, max_nlb);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nvme_lba_count_ok_6533(uint32_t nlb, uint32_t max_nlb)
    __attribute__((alias("gj_nvme_lba_count_ok_6533")));
