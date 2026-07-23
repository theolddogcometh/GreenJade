/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6532: NVMe namespace active soft predicate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_nvme_ns_active_p_6532(uint32_t nsid_ok, uint32_t flags);
 *     - Soft active gate: nsid_ok non-zero and bit0 of flags set → 1;
 *       else 0. Soft stand-in for NSFEAT/ready without hardware.
 *   uint32_t __gj_nvme_ns_active_p_6532  (alias)
 *   __libcgj_batch6532_marker = "libcgj-batch6532"
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

const char __libcgj_batch6532_marker[] = "libcgj-batch6532";

#define B6532_ACTIVE_BIT  0x00000001u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6532_active_p(uint32_t u32NsidOk, uint32_t u32Flags)
{
	if (u32NsidOk == 0u) {
		return 0u;
	}
	if ((u32Flags & B6532_ACTIVE_BIT) == 0u) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nvme_ns_active_p_6532 - soft NVMe namespace active predicate.
 *
 * nsid_ok: non-zero when soft NSID check already passed
 * flags:   soft feature/ready bits (bit0 = active)
 *
 * Returns 1 when active, else 0. Pure integer. No parent wires.
 */
uint32_t
gj_nvme_ns_active_p_6532(uint32_t nsid_ok, uint32_t flags)
{
	(void)NULL;
	return b6532_active_p(nsid_ok, flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nvme_ns_active_p_6532(uint32_t nsid_ok, uint32_t flags)
    __attribute__((alias("gj_nvme_ns_active_p_6532")));
