/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6536: NVMe submission queue size pack stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_nvme_sq_size_pack_6536(uint32_t qdepth, uint32_t esize);
 *     - Soft SQ size word: (qdepth_m1 & 0xffff) | ((esize & 0xffff) << 16)
 *       where qdepth_m1 = qdepth-1 when qdepth >= 1, else 0. Pure fold.
 *   uint32_t __gj_nvme_sq_size_pack_6536  (alias)
 *   __libcgj_batch6536_marker = "libcgj-batch6536"
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

const char __libcgj_batch6536_marker[] = "libcgj-batch6536";

#define B6536_LO16  0x0000ffffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6536_sq_size_pack(uint32_t u32Depth, uint32_t u32Esize)
{
	uint32_t u32M1;

	if (u32Depth == 0u) {
		u32M1 = 0u;
	} else {
		u32M1 = u32Depth - 1u;
	}
	return (u32M1 & B6536_LO16) | ((u32Esize & B6536_LO16) << 16);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nvme_sq_size_pack_6536 - soft NVMe submission queue size fold.
 *
 * qdepth: entry count (encoded as count-1 in low 16 bits)
 * esize:  soft entry size field (low 16 bits retained, shifted high)
 *
 * Returns packed soft SQ size word. Pure integer. No parent wires.
 */
uint32_t
gj_nvme_sq_size_pack_6536(uint32_t qdepth, uint32_t esize)
{
	(void)NULL;
	return b6536_sq_size_pack(qdepth, esize);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nvme_sq_size_pack_6536(uint32_t qdepth, uint32_t esize)
    __attribute__((alias("gj_nvme_sq_size_pack_6536")));
