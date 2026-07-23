/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6531: NVMe namespace id soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_nvme_nsid_ok_6531(uint32_t nsid);
 *     - Soft NSID validity: 0 and 0xffffffff reserved → 0; else 1 when
 *       nsid is in [1, 0xfffffffe].
 *   uint32_t __gj_nvme_nsid_ok_6531  (alias)
 *   __libcgj_batch6531_marker = "libcgj-batch6531"
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

const char __libcgj_batch6531_marker[] = "libcgj-batch6531";

#define B6531_NSID_MAX  0xfffffffeu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6531_nsid_ok(uint32_t u32Nsid)
{
	if (u32Nsid == 0u || u32Nsid > B6531_NSID_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nvme_nsid_ok_6531 - soft NVMe namespace identifier validity.
 *
 * nsid: namespace id (0 and 0xffffffff reserved / broadcast)
 *
 * Returns 1 when nsid is in [1, 0xfffffffe], else 0. Pure data only.
 * No parent wires.
 */
uint32_t
gj_nvme_nsid_ok_6531(uint32_t nsid)
{
	(void)NULL;
	return b6531_nsid_ok(nsid);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nvme_nsid_ok_6531(uint32_t nsid)
    __attribute__((alias("gj_nvme_nsid_ok_6531")));
